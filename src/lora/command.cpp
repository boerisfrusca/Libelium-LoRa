//============================================================================
// Name        : command.cpp
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Lo-Ra commands
//============================================================================
#include "command.h"
#include "utils.h"
#include <ctype.h>
#include <stdlib.h>
#include <string>
#include <locale>

namespace lora
{
  namespace command
  {

////////////////////////////////////// Read /////////////////////////////////////////////////
    Read::Read()
    {
      m_type = READ;
    }

    Read::Read(const Read &cmd) :
        Command(cmd), OutputCommand(cmd)
    {
    }

    Read::~Read()
    {

    }

    uint8_t Read::createFieldType(uint8_t *buffer, uint8_t size)
    {
      if (buffer == 0 || size < SZ_CMD)
        return 0;

      uint8_t index = 0;
      buffer[index++] = 'R';
      buffer[index++] = 'E';
      buffer[index++] = 'A';
      buffer[index++] = 'D';

      return index;
    }

    uint8_t Read::serialize(uint8_t *buffer, size_t size)
    {
      uint8_t msg_size = SZ_START + SZ_CMD + SZ_SEPARATOR + SZ_CRC + SZ_END;

      m_size = 0;

      if (buffer == 0 || size < msg_size)
        return 0;

      uint8_t index = 0;
      uint8_t n = 0;

// Create start field
      if ((n = createFieldStart(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

// Create type field
//    if ((n = createFieldType(&buffer[index], size - index)) == 0)
      if ((n = createPayload(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

      m_crc = CRC16(&buffer[1], index - 1);

// Create separator before CRC
      if ((n = createFieldSeparator(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

// create CRC field
      if ((n = createFieldCRC(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

// Create end field
      if ((n = createFieldEnd(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

// Create separator
      if ((n = createFieldSeparator(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;
      /*
       for (uint8_t i = 0; i < index; i++)
       {
       int c = (buffer[i] & 0x0FF);
       std::cout << "[";
       std::cout << std::dec << (int) i << "]: ";
       std::cout << std::hex << c << std::endl;
       }
       */
      m_size = index;

      return index;
    }

////////////////////////////////////// Info /////////////////////////////////////////////////
    Info::Info()
    {
      m_type = INFO;
    }

    Info::Info(const Info &cmd) :
        ConfigCommand(cmd)
    {

    }

    Info::~Info()
    {

    }

    uint8_t Info::createFieldType(uint8_t *buffer, uint8_t size)
    {
      if (buffer == 0 || size < SZ_CMD)
        return 0;

      uint8_t index = 0;
      buffer[index++] = 'I';
      buffer[index++] = 'N';
      buffer[index++] = 'F';
      buffer[index++] = 'O';

      return index;
    }

    uint8_t Info::createFromBuffer(uint8_t *buffer, size_t size)
    {
      m_type = INFO;
      m_crc = 0;
      m_size = 0;

      if (size == 0 || buffer == NULL)
        return 0;

      enum _state
      {
        LOOKFOR_START,
        LOOKFOR_FREQ,
        LOOKFOR_ADDR,
        LOOKFOR_BW,
        LOOKFOR_CR,
        LOOKFOR_SF,
        LOOKFOR_OPT,
        MSG_RECEIVED
      };

      uint8_t state = LOOKFOR_START;
      std::string field;

      for (size_t i = 0; i < size; i++)
      {
        switch (state)
        {
          case LOOKFOR_START:
          {
            if (i == 0 && buffer[i] != '#')
              return 0;
            state = LOOKFOR_FREQ;
          }
            break;

          case LOOKFOR_FREQ:
          {
            if (buffer[i] != ';')
              field += buffer[i];
            else
            {
              uint16_t fr = 0;
              uint8_t ch = 0;

              toUpper(field);

              if (!processField(field, "FREC", ":", field))
                return 0;

              if (!processField(field, "CH", "_", field))
                return 0;

              // Get channel
              std::string token = field.substr(0, field.find('_'));
              field.erase(0, token.size() + 1);

              if (is_number(field))
              {
                long v = atol(field.c_str());
                fr = (v & 0x0FFFF);
              }

              if (is_number(token))
              {
                int v = atoi(token.c_str());
                ch = (v & 0x0FF);
              }

              switch (fr)
              {
                case 868:
                {
                  switch (ch)
                  {
                    case 10:
                      setChannel(CH_10);
                      break;

                    case 11:
                      setChannel(CH_11);
                      break;

                    case 12:
                      setChannel(CH_12);
                      break;

                    case 13:
                      setChannel(CH_13);
                      break;

                    case 14:
                      setChannel(CH_14);
                      break;

                    case 15:
                      setChannel(CH_15);
                      break;

                    case 16:
                      setChannel(CH_16);
                      break;

                    case 17:
                      setChannel(CH_17);
                      break;

                    default:
                      setChannel(CH_UNKN);
                      setFrequency(F_UNKN);
                      return 0;
                  }

                  setFrequency(F_868);
                }
                  break;

                case 900:
                {
                  switch (ch)
                  {
                    case 0:
                      setChannel(CH_00);
                      break;

                    case 1:
                      setChannel(CH_01);
                      break;

                    case 2:
                      setChannel(CH_02);
                      break;

                    case 3:
                      setChannel(CH_03);
                      break;

                    case 4:
                      setChannel(CH_04);
                      break;

                    case 5:
                      setChannel(CH_05);
                      break;

                    case 6:
                      setChannel(CH_06);
                      break;

                    case 7:
                      setChannel(CH_07);
                      break;

                    case 8:
                      setChannel(CH_08);
                      break;

                    case 9:
                      setChannel(CH_09);
                      break;

                    case 10:
                      setChannel(CH_10);
                      break;

                    case 11:
                      setChannel(CH_11);
                      break;

                    case 12:
                      setChannel(CH_12);
                      break;

                    default:
                      setChannel(CH_UNKN);
                      setFrequency(F_UNKN);
                      return 0;
                  }

                  setFrequency(F_900);
                }
                  break;

                default:
                  setChannel(CH_UNKN);
                  setFrequency(F_UNKN);
                  return 0;
              }

              field = "";
              state = LOOKFOR_ADDR;
            }
          }
            break;

          case LOOKFOR_ADDR:
          {
            if (buffer[i] != ';')
              field += buffer[i];
            else
            {
              toUpper(field);

              long addr = 0;
              std::string value;
              if (!processField(field, "ADDR", ":", addr))
                return 0;

              addr &= 0x0FF;
              setAddress(addr);

              field = "";
              state = LOOKFOR_BW;
            }
          }
            break;

          case LOOKFOR_BW:
          {
            if (buffer[i] != ';')
              field += buffer[i];
            else
            {
              toUpper(field);

              std::string value;
              if (!processField(field, "BW", ":", value))
                return 0;

              if (value == "BW_125")
              {
                setBandwidth(BW_125);
              }
              else if (value == "BW_250")
                setBandwidth(BW_250);
              else if (value == "BW_500")
                setBandwidth(BW_500);
              else
                setBandwidth(BW_UNKN);

              field = "";
              state = LOOKFOR_CR;
            }
          }
            break;

          case LOOKFOR_CR:
          {
            if (buffer[i] != ';')
              field += buffer[i];
            else
            {
              toUpper(field);

              std::string value;
              if (!processField(field, "CR", ":", value))
                return 0;

              if (value == "CR_5")
              {
                setCodingRate(CR_5);
              }
              else if (value == "CR_6")
                setCodingRate(CR_6);
              else if (value == "CR_7")
                setCodingRate(CR_7);
              else if (value == "CR_8")
                setCodingRate(CR_8);
              else
                setCodingRate(CR_UNKN);

              field = "";
              state = LOOKFOR_SF;
            }
          }
            break;

          case LOOKFOR_SF:
          {
            if (buffer[i] != ';')
              field += buffer[i];
            else
            {
              toUpper(field);

              std::string value;
              if (!processField(field, "SF", ":", value))
                return 0;

              if (value == "SF_6")
                setSpreadingFactor(SF_6);
              else if (value == "SF_7")
                setSpreadingFactor(SF_7);
              else if (value == "SF_8")
                setSpreadingFactor(SF_8);
              else if (value == "SF_9")
                setSpreadingFactor(SF_9);
              else if (value == "SF_10")
                setSpreadingFactor(SF_10);
              else if (value == "SF_8")
                setSpreadingFactor(SF_11);
              else if (value == "SF_12")
                setSpreadingFactor(SF_12);
              else
                setSpreadingFactor(SF_UNKN);

              field = "";
              state = LOOKFOR_OPT;
            }
          }
            break;

          case LOOKFOR_OPT:
          {

            if (buffer[i] == EOT)
            {
              state = MSG_RECEIVED;
              break;
            }

            if ((buffer[i] != ';') && (i < (size - 1)))
              field += buffer[i];
            else
            {
              if (i == (size - 1))
                field += buffer[i];

              toUpper(field);

              long value = 0;
              if (processField(field, "RSSI", ":", value))
              {
                m_rssi = value;
                field = "";
              }
              if (processField(field, "SNR", ":", value))
              {
                m_snr = value;
                field = "";
              }
              if (processField(field, "RSSI_PACKET", ":", value))
              {
                m_rssi_pck = value;
                field = "";
              }
            }
          }

        }
      }

      return 1;

    }

////////////////////////////////////// Set /////////////////////////////////////////////////
    Set::Set()
    {
      m_type = SET;
    }

    Set::Set(const Set &cmd) :
        ConfigCommand(cmd)
    {

    }

    Set::~Set()
    {

    }

    uint8_t Set::createFieldType(uint8_t *buffer, uint8_t size)
    {
      if (buffer == 0 || size < SZ_CMD)
        return 0;

      uint8_t index = 0;
      buffer[index++] = 'S';
      buffer[index++] = 'E';
      buffer[index++] = 'T';

      return index;
    }

    uint8_t Set::createPayload(uint8_t *buffer, size_t size)
    {
      uint8_t index = 0;
      uint8_t n = 0;

      // Create type field
      if ((n = createFieldType(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

      std::string pl = "#";

      // Frequency:
      pl += "FREC:CH_";
      std::string fld = "";
      convertIntToSting(m_ch, fld);
      pl += fld;
      pl += "_";
      switch (m_freq)
      {
        case F_868:
          convertIntToSting(868, fld);
          break;
        case F_900:
          convertIntToSting(900, fld);
          break;
        default:
          return 0;
      }
      pl += fld;
      pl += ";";

      // Address
      pl += "ADDR:";
      convertIntToSting(m_addr, fld);
      pl += fld;
      pl += ";";

      // Bandwidth:
      pl += "BW:BW_";
      switch (m_bw)
      {
        case BW_125:
          convertIntToSting(125, fld);
          break;
        case BW_250:
          convertIntToSting(250, fld);
          break;
        case BW_500:
          convertIntToSting(500, fld);
          break;
        default:
          return 0;
      }
      pl += fld;
      pl += ";";

      // Coding Rate:
      pl += "CR:CR_";
      switch (m_cr)
      {
        case CR_5:
          convertIntToSting(5, fld);
          break;
        case CR_6:
          convertIntToSting(6, fld);
          break;
        case CR_7:
          convertIntToSting(7, fld);
          break;
        case CR_8:
          convertIntToSting(8, fld);
          break;
        default:
          return 0;
      }
      pl += fld;
      pl += ";";

      // Spreading factor:
      pl += "SF:SF_";
      switch (m_sf)
      {
        case SF_6:
          convertIntToSting(6, fld);
          break;
        case SF_7:
          convertIntToSting(7, fld);
          break;
        case SF_8:
          convertIntToSting(8, fld);
          break;
        case SF_9:
          convertIntToSting(9, fld);
          break;
        case SF_10:
          convertIntToSting(10, fld);
          break;
        case SF_11:
          convertIntToSting(11, fld);
          break;
        case SF_12:
          convertIntToSting(12, fld);
          break;
        default:
          return 0;
      }
      pl += fld;

      for (size_t i = 0; index < size && i < pl.size(); index++, i++)
      {
        buffer[index] = pl[i];
      }

      return index;
    }

    uint8_t Set::serialize(uint8_t *buffer, size_t size)
    {
      uint8_t msg_size = SZ_START + SZ_CMD + SZ_SEPARATOR + SZ_CRC + SZ_END;

      m_size = 0;

      if (buffer == 0 || size < msg_size)
        return 0;

      if (m_freq == F_UNKN || m_ch == CH_UNKN)
        return 0;

      uint8_t index = 0;
      uint8_t n = 0;

      // Create start field
      if ((n = createFieldStart(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;
      // Create payload field
      if ((n = createPayload(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

      // Calculate CRC
      m_crc = CRC16(&buffer[1], index - 1);

      // Create separator before CRC
      if ((n = createFieldSeparator(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

      // create CRC field
      if ((n = createFieldCRC(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

      // Create end field
      if ((n = createFieldEnd(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

      m_size = index;

      return index;
    }

    ////////////////////////////////////// Error ////////////////////////////////////////////////
    Error::Error()
    {
      m_type = ERROR;
    }

    Error::Error(const Error &cmd) :
        Command(cmd)
    {
      m_error = cmd.m_error;
    }

    Error::~Error()
    {

    }

    uint8_t Error::createFieldType(uint8_t *buffer, uint8_t size)
    {
      if (buffer == 0 || size < SZ_CMD)
        return 0;

      uint8_t index = 0;
      buffer[index++] = 'E';
      buffer[index++] = 'R';
      buffer[index++] = 'R';
      buffer[index++] = 'O';
      buffer[index++] = 'R';

      return index;
    }

    uint8_t Error::createFromBuffer(uint8_t *buffer, size_t size)
    {
      m_type = ERROR;
      m_crc = 0;
      m_size = 0;

      if (size == 0 || buffer == NULL)
        return 0;

      enum _state
      {
        LOOKFOR_START, LOOKFOR_DATA, MSG_RECEIVED
      };

      uint8_t state = LOOKFOR_START;
      std::string field;

      for (uint8_t i = 0; i < size; i++)
      {
        switch (state)
        {
          case LOOKFOR_START:
          {
            if (i == 0 && buffer[i] != '#')
              return 0;
            state = LOOKFOR_DATA;
          }
            break;

          case LOOKFOR_DATA:
          {
            if (buffer[i] == EOT)
            {
              state = MSG_RECEIVED;
              break;
            }
            else
              field += buffer[i];
          }
            break;

          case MSG_RECEIVED:
          {
            m_error = field;
          }
            break;

          default:
            return 0;
        }
      }

      return 1;

    }

    ////////////////////////////////////// Data ////////////////////////////////////////////////
    Data::Data() :
        m_dest(0)
    {
      m_type = DATA;
    }

    Data::Data(const Data &cmd) :
        Command(cmd)
    {
      m_dest = cmd.m_dest;
      m_data = cmd.m_data;
    }

    Data::~Data()
    {

    }

    uint8_t Data::createFieldType(uint8_t *buffer, uint8_t size)
    {
      if (buffer == 0 || size < SZ_CMD)
        return 0;

      uint8_t index = 0;
      buffer[index++] = 'D';
      buffer[index++] = 'A';
      buffer[index++] = 'T';
      buffer[index++] = 'A';

      return index;
    }

    uint8_t Data::createPayload(uint8_t *buffer, size_t size)
    {
      uint8_t index = 0;
      uint8_t n = 0;

      // Create type field
      if ((n = createFieldType(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

      std::string addr = "";
      convertIntToSting(m_dest, addr);
      std::string pl = "#" + addr + "#ASCII#" + m_data;

      for (size_t i = 0; index < size && i < pl.size(); index++, i++)
      {
        buffer[index] = pl[i];
      }

      return index;
    }

    uint8_t Data::serialize(uint8_t *buffer, size_t size)
    {
      uint8_t msg_size = SZ_START + SZ_CMD + SZ_SEPARATOR + SZ_CRC + SZ_END;

      m_size = 0;

      if (buffer == 0 || size < msg_size)
        return 0;

      uint8_t index = 0;
      uint8_t n = 0;

      // Create start field
      if ((n = createFieldStart(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

      // Create payload field
      if ((n = createPayload(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

      // Calculate CRC
      m_crc = CRC16(&buffer[1], index - 1);

      // Create separator before CRC
      if ((n = createFieldSeparator(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

      // create CRC field
      if ((n = createFieldCRC(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

      // Create end field
      if ((n = createFieldEnd(&buffer[index], size - index)) == 0)
        return 0;
      else
        index += n;

      m_size = index;

      return index;

    }

////////////////////////////////////// Ack ////////////////////////////////////////////////
    Ack::Ack()
    {
      m_type = ACK;
    }

    Ack::Ack(const Ack &cmd) :
        Command(cmd)
    {
    }

    Ack::~Ack()
    {

    }

    uint8_t Ack::createFieldType(uint8_t *buffer, uint8_t size)
    {
      if (buffer == 0 || size < SZ_CMD)
        return 0;

      uint8_t index = 0;
      buffer[index++] = 'A';
      buffer[index++] = 'C';
      buffer[index++] = 'K';

      return index;
    }

    uint8_t Ack::createFromBuffer(uint8_t *buffer, size_t size)
    {
      return 1;
    }

    uint8_t Ack::createPayload(uint8_t *buffer, size_t size)
    {
      return 1;
    }

  } /* namespace command */
} /* namespace lora */
