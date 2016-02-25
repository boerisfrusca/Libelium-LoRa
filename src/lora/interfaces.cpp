//============================================================================
// Name        : interfaces.cpp
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Interface for Lo-Ra commands
//============================================================================
#include "interfaces.h"
#include "utils.h"
#include <stdlib.h>
#include <iostream>

namespace lora
{
  /*******************************************************************************************************************
   * Command
   *******************************************************************************************************************/
  uint16_t Command::CRC16(uint8_t *buf, size_t len)
  {
    uint16_t crc = 0xFFFF;

    for (size_t pos = 0; pos < len; pos++)
    {
      crc ^= (uint16_t) buf[pos];       // XOR byte into least sig. byte of crc

      for (int i = 8; i != 0; i--)
      {    // Loop over each bit
        if ((crc & 0x0001) != 0)
        {      // If the LSB is set
          crc >>= 1;                    // Shift right and XOR 0xA001
          crc ^= 0xA001;
        }
        else
          // Else LSB is not set
          crc >>= 1;                    // Just shift right
      }
    }
    // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
    return crc;
  }

  uint8_t Command::createFieldStart(uint8_t *buffer, uint8_t size)
  {
    if (buffer == 0 || size < SZ_START)
      return 0;

    uint8_t index = 0;
    buffer[index++] = SOH;

    return index;

  }

  uint8_t Command::createFieldCRC(uint8_t *buffer, uint8_t size)
  {
    if (buffer == 0 || size < SZ_CRC /* sizeof(m_crc) */)
      return 0;

    uint8_t index = 0;

    for (uint8_t i = 0; i < SZ_CRC_BINARY/* ( sizeof(m_crc) * 2 ) */; i++)
    {
      uint8_t low = 0;
      uint8_t high = 0;
      uint8_t c = 0;

      if ((i % 2) == 0)
        c = (m_crc & 0xFF00) >> 8;
      else
        c = m_crc & 0x0FF;

      convertHexToChars(c, high, low);
      buffer[index++] = high;
      buffer[index++] = low;
    }

    return index;

  }

  uint8_t Command::createFieldSeparator(uint8_t *buffer, uint8_t size)
  {
    if (buffer == 0 || size < SZ_SEPARATOR)
      return 0;

    uint8_t index = 0;
    buffer[index++] = CR;
    buffer[index++] = LF;

    return index;

  }

  uint8_t Command::createFieldEnd(uint8_t *buffer, uint8_t size)
  {
    if (buffer == 0 || size < SZ_END)
      return 0;

    uint8_t index = 0;
    buffer[index++] = EOT;

    return index;

  }

  bool Command::processField(const std::string &field, const std::string &tag,
      const std::string &fs, std::string &value)
  {
    // Parse ID field
    std::string str = field;
    std::string token = field.substr(0, field.find(fs));

    if (token == tag)
    {
      str.erase(0, token.size() + 1);
      value = str;
    }
    else
    {
      return false;
    }

    return true;
  }

  bool Command::processField(const std::string &field, const std::string &tag,
      const std::string &fs, long &value)
  {
    // Parse ID field
    std::string str = field;
    std::string token = field.substr(0, field.find(fs));

    if (token == tag)
    {
      str.erase(0, token.size() + 1);
    }
    else
    {
      return false;
    }

    // Get coding rate
    if (is_number(str))
    {
      value = atol(str.c_str());
    }
    else
      return false;

    return true;
  }

  uint8_t Command::process(const uint8_t *buffer, const size_t sz, uint8_t &type, uint8_t *payload,
      size_t &p_size, uint16_t &crc)
  {
    enum _state
    {
      LOOKFOR_SOH,
      LOOKFOR_CMD,
      LOOKFOR_CMD_ACK,
      LOOKFOR_CMD_DATA,
      LOOKFOR_CMD_ERROR,
      LOOKFOR_CMD_INFO,
      LOOKFOR_CMD_READ,
      LOOKFOR_CMD_SET,
      LOOKFOR_PAYLOAD,
      LOOKFOR_SEPARATOR,
      LOOKFOR_CRC,
      LOOKFOR_EOT,
      MSG_NOT_FOUND,
      MSG_FOUND,
    };

    uint8_t ret = NO_ERROR;

    size_t index = 0;
    size_t i = 0;

    crc = 0;

    std::string cmd = "";

    if (buffer == 0)
    {
      return NULL_BUFFER_IN;
    }

    if (payload == 0)
    {
      return NULL_BUFFER_OUT;
    }

    bool msg_found = false;
    uint8_t state = LOOKFOR_SOH;

    while (index < sz && !msg_found)
    {

      uint16_t c = buffer[index];
      c &= 0x0FF;

      //std::cerr << "Process ";
      //std::cerr << std::dec << index;
      //std::cerr << " : ";
      //std::cerr << std::hex << c << std::endl;

      switch (state)
      {
        case LOOKFOR_SOH:
        {
          if (buffer[index] == SOH)
          {
            i = 0;
            state = LOOKFOR_CMD;
          }
        }
          break;

        case LOOKFOR_CMD:
        {

          if (buffer[index] == '#' || buffer[index] == CR)
          {
            index--;
            p_size = 0;
            i = 0;
            state = LOOKFOR_PAYLOAD;

            if (cmd == "ACK")
            {
              type = ACK;
            }
            else if (cmd == "DATA")
            {
              type = DATA;
            }
            else if (cmd == "ERROR")
            {
              type = ERROR;
            }
            else if (cmd == "INFO")
            {
              type = INFO;
            }
            else if (cmd == "READ")
            {
              type = READ;
            }
            else if (cmd == "SET")
            {
              type = SET;
            }
            else
            {
              ret = INVALID_CMD;
              state = MSG_NOT_FOUND;
            }

          }
          else
          {
            cmd += toupper(buffer[index]);
          }
        }
          break;

        case LOOKFOR_PAYLOAD:
        {

          if (index == 0)
          {
            ret = INVALID_PAYLOAD_1;
            state = MSG_NOT_FOUND;
          }
          else
          {
            // Check if sequence CR+LF is found
            if (buffer[index - 1] == CR && buffer[index] == LF)
            {
              if (i > 0)
              {
                i--;

                payload[i] = 0;
                p_size = i;
                i = 0;
                state = LOOKFOR_CRC;
              }
              else
              {
                ret = INVALID_PAYLOAD_2;
                state = MSG_NOT_FOUND;
              }
            }
            else
            {
              payload[i++] = buffer[index];
            }
          }

        }
          break;

        case LOOKFOR_CRC:
        {
          switch (i++)
          {
            case 0:
            case 1:
            case 2:
            case 3:
            {

              uint16_t c = 0;
              uint8_t v = convertHexCharToInt(buffer[index]);

              if (v == 0xFF)
              {
                ret = INVALID_CRC;
                state = MSG_NOT_FOUND;
              }

              if (state != MSG_NOT_FOUND)
              {
                c = v;
                c &= 0x0F;
                c <<= ((4 - i) * 4);
                crc |= c;

                if (i == 4)
                {
                  state = LOOKFOR_EOT;
                  i = 0;
                }
              }
            }
              break;
          }
        }
          break;

        case LOOKFOR_EOT:
        {
          if (buffer[index] == EOT)
          {
            i = 0;
            state = MSG_FOUND;
          }
          else
          {
            ret = INVALID_EOT;
            state = MSG_NOT_FOUND;
          }
        }
          break;

        case MSG_FOUND:
        {
          msg_found = true;
        }
          break;

        case MSG_NOT_FOUND:
        {
          type = UNKNOWN;
          p_size = 0;
          crc = 0;

          return ret;
        }
      }

      if (state != MSG_FOUND)
        index++;
    }

    if (state != MSG_FOUND)
    {
      ret = CMD_NOT_FOUND;
    }
    else
    {

    }

    return ret;
  }

  uint8_t Command::createPayload(uint8_t *buffer, size_t size)
  {
    uint8_t index = 0;
    uint8_t n = 0;

    // Create type field
    if ((n = createFieldType(&buffer[index], size - index)) == 0)
      return 0;
    else
      index += n;

    return index;
  }

  /*******************************************************************************************************************
   * Command
   *******************************************************************************************************************/

  ConfigCommand::ConfigCommand() :
      m_freq(F_UNKN), m_ch(CH_UNKN), m_addr(0), m_bw(BW_UNKN), m_cr(CR_UNKN), m_sf(SF_UNKN), m_rssi(
          0), m_rssi_pck(0), m_snr(0)
  {
  }

  ConfigCommand::ConfigCommand(const ConfigCommand &cmd) :
      Command(cmd)
  {
    m_freq = cmd.m_freq;
    m_ch = cmd.m_ch;
    m_addr = cmd.m_addr;
    m_bw = cmd.m_bw;
    m_cr = cmd.m_cr;
    m_sf = cmd.m_sf;
    m_rssi = cmd.m_rssi;
    m_snr = cmd.m_snr;
    m_rssi_pck = cmd.m_rssi_pck;
  }

  ConfigCommand::~ConfigCommand()
  {

  }

  uint16_t ConfigCommand::frequency(bool code)
  {
    uint16_t ret = m_freq;
    if (!code)
    {
      switch (m_freq)
      {
        case F_868:
          ret = 868;
          break;
        case F_900:
          ret = 900;
          break;
        case F_UNKN:
        default:
          ret = 0;
          break;
      }
    }
    return ret;
  }

  std::string ConfigCommand::frequencyAsString()
  {
    std::string ret = "";
    switch (m_freq)
    {
      case F_868:
        ret = "868";
        break;
      case F_900:
        ret = "900";
        break;
      case F_UNKN:
      default:
        ret = "Unknown";
        break;
    }
    return ret;
  }

  bool ConfigCommand::setFrequency(uint8_t freq)
  {
    switch (freq)
    {
      case F_868:
      case F_900:
      case F_UNKN:
      {
        m_freq = freq;
      }
        break;
      default:
      {
        m_freq = F_UNKN;
        return false;
      }
    }
    return true;
  }

  uint8_t ConfigCommand::channel(bool code)
  {
    uint16_t ret = m_ch;

    if (!code)
    {
      switch (m_ch)
      {
        case CH_00:
          ret = 0;
          break;
        case CH_01:
          ret = 1;
          break;
        case CH_02:
          ret = 2;
          break;
        case CH_03:
          ret = 3;
          break;
        case CH_04:
          ret = 4;
          break;
        case CH_05:
          ret = 5;
          break;
        case CH_06:
          ret = 6;
          break;
        case CH_07:
          ret = 7;
          break;
        case CH_08:
          ret = 8;
          break;
        case CH_09:
          ret = 9;
          break;
        case CH_10:
          ret = 10;
          break;
        case CH_11:
          ret = 11;
          break;
        case CH_12:
          ret = 12;
          break;
        case CH_13:
          ret = 13;
          break;
        case CH_14:
          ret = 14;
          break;
        case CH_15:
          ret = 15;
          break;
        case CH_16:
          ret = 16;
          break;
        case CH_17:
          ret = 17;
          break;
        case CH_UNKN:
        default:
          ret = 0;
          break;
      }
    }
    return ret;
  }

  std::string ConfigCommand::channelAsString()
  {
    std::string ret = "";
    switch (m_ch)
    {
      case CH_00:
        ret = "0";
        break;
      case CH_01:
        ret = "1";
        break;
      case CH_02:
        ret = "2";
        break;
      case CH_03:
        ret = "3";
        break;
      case CH_04:
        ret = "4";
        break;
      case CH_05:
        ret = "5";
        break;
      case CH_06:
        ret = "6";
        break;
      case CH_07:
        ret = "7";
        break;
      case CH_08:
        ret = "8";
        break;
      case CH_09:
        ret = "9";
        break;
      case CH_10:
        ret = "10";
        break;
      case CH_11:
        ret = "11";
        break;
      case CH_12:
        ret = "12";
        break;
      case CH_13:
        ret = "13";
        break;
      case CH_14:
        ret = "14";
        break;
      case CH_15:
        ret = "15";
        break;
      case CH_16:
        ret = "16";
        break;
      case CH_17:
        ret = "17";
        break;
      case CH_UNKN:
      default:
        ret = "Unknown";
        break;
    }
    return ret;
  }

  bool ConfigCommand::setChannel(uint8_t ch)
  {
    switch (ch)
    {
      case CH_00:
      case CH_01:
      case CH_02:
      case CH_03:
      case CH_04:
      case CH_05:
      case CH_06:
      case CH_07:
      case CH_08:
      case CH_09:
      case CH_10:
      case CH_11:
      case CH_12:
      case CH_13:
      case CH_14:
      case CH_15:
      case CH_16:
      case CH_17:
      case CH_UNKN:
      {
        m_ch = ch;
      }
        break;
      default:
      {
        m_ch = CH_UNKN;
        return false;
      }
    }
    return true;
  }

  uint16_t ConfigCommand::bandwidth(bool code)
  {
    uint16_t ret = m_bw;

    if (!code)
    {
      switch (m_bw)
      {
        case BW_125:
          ret = 125;
          break;
        case BW_250:
          ret = 250;
          break;
        case BW_500:
          ret = 500;
          break;
        case BW_UNKN:
        default:
          ret = 0;
          break;
      }
    }
    return ret;
  }

  std::string ConfigCommand::bandwidthAsString()
  {
    std::string ret = "";
    switch (m_bw)
    {
      case BW_125:
        ret = "125";
        break;
      case BW_250:
        ret = "250";
        break;
      case BW_500:
        ret = "500";
        break;
      case BW_UNKN:
      default:
        ret = "Unknown";
        break;
    }
    return ret;
  }

  bool ConfigCommand::setBandwidth(uint8_t bw)
  {
    switch (bw)
    {
      case BW_125:
      case BW_250:
      case BW_500:
      case BW_UNKN:
      {
        m_bw = bw;
      }
        break;
      default:
      {
        m_bw = BW_UNKN;
        return false;
      }
    }
    return true;
  }

  uint8_t ConfigCommand::codingRate(bool code)
  {
    uint16_t ret = m_cr;

    if (!code)
    {
      switch (m_cr)
      {
        case CR_5:
          ret = 5;
          break;
        case CR_6:
          ret = 6;
          break;
        case CR_7:
          ret = 7;
          break;
        case CR_8:
          ret = 8;
          break;
        case CR_UNKN:
        default:
          ret = 0;
          break;
      }
    }
    return ret;
  }

  std::string ConfigCommand::codingRateAsString()
  {
    std::string ret = "";
    switch (m_cr)
    {
      case CR_5: ret = "5";  break;
      case CR_6: ret = "6";  break;
      case CR_7: ret = "7";  break;
      case CR_8: ret = "8";  break;
      case CH_UNKN:
      default:
        ret = "Unknown";
        break;
    }
    return ret;
  }

  bool ConfigCommand::setCodingRate(uint8_t cr)
  {
    switch (cr)
    {
      case CR_5:
      case CR_6:
      case CR_7:
      case CR_8:
      case CR_UNKN:
      {
        m_cr = cr;
      }
        break;
      default:
      {
        m_cr = CR_UNKN;
        return false;
      }
    }

    return true;
  }

  uint8_t ConfigCommand::spreadingFactor(bool code)
  {
    uint16_t ret = m_sf;

    if (!code)
    {
      switch (m_sf)
      {
        case SF_6:  ret = 6; break;
        case SF_7:  ret = 7; break;
        case SF_8:  ret = 8; break;
        case SF_9:  ret = 9; break;
        case SF_10: ret = 10; break;
        case SF_11: ret = 11; break;
        case SF_12: ret = 12; break;
        case SF_UNKN:
        default:
          ret = 0;
          break;
      }
    }
    return ret;
  }

  std::string ConfigCommand::spreadingFactorAsString()
  {
    std::string ret = "";
    switch (m_sf)
    {
      case SF_6: ret = "6";  break;
      case SF_7: ret = "7";  break;
      case SF_8: ret = "8";  break;
      case SF_9: ret = "9";  break;
      case SF_10: ret = "10";  break;
      case SF_11: ret = "11";  break;
      case SF_12: ret = "12";  break;
      case SF_UNKN:
      default:
        ret = "Unknown";
        break;
    }
    return ret;

  }

  bool ConfigCommand::setSpreadingFactor(uint8_t sf)
  {
    switch (sf)
    {
      case SF_6:
      case SF_7:
      case SF_8:
      case SF_9:
      case SF_10:
      case SF_11:
      case SF_12:
      case SF_UNKN:
      {
        m_sf = sf;
      }
        break;
      default:
      {
        m_sf = CR_UNKN;
        return false;
      }
    }
    return true;
  }

} /* namespace lora */

