//============================================================================
// Name        : global.cpp
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Global functions
//============================================================================
#include "global.h"
#include "lora/utils.h"
#include "lora/serial.h"
#include "lora/command.h"

/*****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
std::string msg_string(uint8_t *buffer, size_t sz)
{
  std::string msg =  "";
  for (uint16_t i = 0; i < sz; i++)
  {
    switch (buffer[i])
    {
      case lora::Command::SOH:
      {

        msg += "<SOH>";
      }
        break;

      case lora::Command::CR:
      {
        msg += "<CR>";
      }
        break;

      case lora::Command::LF:
      {
        msg += "<LF>";
      }
        break;

      case lora::Command::EOT:
      {
        msg += "<EOT>";
      }
        break;

      default:
        msg +=  buffer[i];
    }
  }
  return msg;
}

bool openSerial(lora::Serial &serial)
{
  // Open serial connection
  V_DEBUG ("Open serial device [%s]\n", serial.device().c_str());

  try
  {
    serial.openDev();
  }
  catch (lora::Serial::Exception &e)
  {
    V_DEBUG ("%s\n", e.what());
    return false;
  }
  return true;
}

bool closeSerial(lora::Serial &serial)
{
  // Close serial connection
  V_DEBUG ("Close serial device [%s]\n", serial.device().c_str());

  try
  {
    serial.closeDev();
  }
  catch (lora::Serial::Exception &e)
  {
    V_DEBUG ("%s\n", e.what());

    return false;
  }
  return true;
}


void process_buffer(uint8_t *rx_buffer, size_t sz)
{
  uint8_t payload[buf_sz] = { 0 };
  uint8_t type = 0;
  size_t psize = 0;
  uint16_t crc = 0;

  V_DEBUG("COMMAND: %s\n", msg_string(rx_buffer, sz).c_str());

  uint8_t ret = lora::Command::process((uint8_t *) rx_buffer, (size_t) sz, type,
      (uint8_t *) payload, psize, crc);

  switch (ret)
  {
    case lora::Command::NO_ERROR:
    {
      V_INFO("Received command\n", type);
      V_INFO("Type    : %d\n", type);
      if (psize)
        V_INFO("Payload : %s\n", payload);
      V_INFO("CRC     : %x\n", crc);

      switch (type)
      {
        case lora::Command::INFO:
        {
          V_INFO("Command type is INFO\n");

          lora::command::Info m;
          m.createFromBuffer((uint8_t *) payload, psize);

          std::cout << "Current configuration:" << std::endl;
          //std::cout << "\tType    : " << std::dec << (int) m.type() << std::endl;
          std::cout << "\tAddr    : " << std::dec << (int) m.address() << std::endl;
          std::cout << "\tFreq    : " << m.frequencyAsString() << " MHz" << std::endl;
          std::cout << "\tChan    : " << m.channelAsString() << std::endl;
          std::cout << "\tBW      : " << m.bandwidthAsString() << " KHz" << std::endl;
          std::cout << "\tCR      : " << m.codingRateAsString() << std::endl;
          std::cout << "\tSF      : " << m.spreadingFactorAsString() << std::endl;
          std::cout << "\tSNR     : " << std::dec << (int) m.snr() << std::endl;
          std::cout << "\tRSSI    : " << std::dec << (int) m.rssi() << std::endl;
          std::cout << "\tRSSI PCK: " << std::dec << (int) m.rssi_pck() << std::endl;
        }
          break;

        case lora::Command::ERROR:
        {
          V_INFO("Command type is ERROR\n");
          lora::command::Error m;
          m.createFromBuffer((uint8_t *) payload, psize);

          std::cout << "Lo-Ra error : " << m.error() << std::endl;
        }
          break;

        case lora::Command::ACK:
        {
          V_INFO("Command type is ACK\n");

          std::cout << "Lo-Ra ACK received" << std::endl;
        }
          break;
      }

    }
      break;

    case lora::Command::CMD_NOT_FOUND:
    {
      std::cout << "Message not Found!" << std::endl;
    }
      break;

    case lora::Command::NULL_BUFFER_IN:
    {
      std::cerr << "Error: Invalid packet buffer: NULL!" << std::endl;

    }
      break;

    case lora::Command::NULL_BUFFER_OUT:
    {
      std::cerr << "Error: Invalid payload buffer: NULL!" << std::endl;

    }
      break;

    case lora::Command::INVALID_CMD:
    {
      std::cerr << "Error: invalid command code!" << std::endl;
    }
      break;

    case lora::Command::INVALID_PAYLOAD_1:
    {
      std::cerr << "Error 1 processing payload and searching CR+LF!" << std::endl;
    }
      break;

    case lora::Command::INVALID_PAYLOAD_2:
    {
      std::cerr << "Error 2 processing payload and searching CR+LF!" << std::endl;
    }
      break;

    case lora::Command::INVALID_CRC:
    {
      std::cerr << "Error: invalid CRC format!" << std::endl;
    }
      break;

    case lora::Command::INVALID_EOT:
    {

    }
      break;

  }
}

void rx_buffer_flush (lora::Serial &serial)
{
  V_INFO("Flush serial receiver buffer\n");

  time_t now;
  time_t end;
  size_t n = 0;


  time(&now);
  end = now + FLUSH_TIMEOUT;

  uint8_t rx_buffer[10];
  while ((now < end) || n)
  {

    ssize_t nb = 10;
    n = serial.receive((char*) rx_buffer, nb);
    //V_DEBUG("%d %s\n", n, rx_buffer);
    usleep(100);
    time(&now);
  }

}
