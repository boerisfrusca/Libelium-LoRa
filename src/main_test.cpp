//============================================================================
// Name        : main_test.cpp
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Header of the main for the "Lo-Ra configuration reader"
//============================================================================
#include <iostream>
#include <string>
#include <cstring>
#include <stdarg.h>
#include <stdlib.h>
#include "global.h"
#include "verbose.h"
#include "main_test.h"
#include "lora/utils.h"
#include "lora/serial.h"
#include "lora/command.h"

#ifdef LORA_TEST

/*****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
int main_test(int argc, char **argv)
{
  int opt = 0;
  std::string msg;
  std::string device = SERIAL_DEVICE;
  unsigned long bitrate = SERIAL_BITRATE;

  // Serial device handler
  lora::Serial serial;

  // Parse command line
  while ((opt = getopt(argc, argv, "v:b:d:h")) != -1)
  {
    switch (opt)
    {

        // Serial bitrate
        case 'b':
        {
          if (is_number(optarg))
          {
            std::stringstream ss(optarg);
            ss >> bitrate;
          }
          else
          {
            std::cerr << "Error: Invalid serial bitrate!" << std::endl;
            std::cerr << "Type '" << LORA_NAME << " -h' for help." << std::endl;
            return 0;
          }
        }
          break;

        // Serial device
        case 'd':
        {
          device = optarg;
        }
          break;

      // Print help
      case 'h':
        print_help();
        return 1;

      case 'v':
         // Verbose level
         v_verbosity(atoi(optarg));
         break;
      default:
        std::cerr << "Type '" << LORA_NAME << "-h' for help." << std::endl;
        std::cerr << std::endl;
        return 0;
    }
  }

  V_DEBUG("Serial device : %s\n",  device.c_str() );
  V_DEBUG("Serial bitrate: %ld\n", bitrate );

  try
  {
    serial.setDevice(device);
    serial.setBitrate(bitrate);
  }
  catch (lora::Serial::Exception &e)
  {
    std::string msg("Error (serial connection): ");
    msg += std::string(e.what());
    msg += std::string("\n");
    std::cerr << msg.c_str() << std::endl;

    return 0;
  }

  V_INFO("Open serial device\n");
  if (openSerial(serial))
  {

    time_t now;
    time_t end;

    size_t t = 0;
    size_t n = 0;

    uint8_t tx_buffer[buf_sz] = { 0 };
    uint8_t rx_buffer[buf_sz] = { 0 };

    // Empty Rx serial buffer
    rx_buffer_flush (serial);

    // Prepare command
    ssize_t sz = 0;

/*
    switch (cmd)
    {
      case CMD_READ:
      {
*/
        // Create SET message
    V_INFO("Create READ command\n");
    lora::command::Read cmd;
    sz = cmd.serialize(tx_buffer, buf_sz);
/*
      }
        break;

      case CMD_SET:
      {
        lora::command::Set set;

        // Create READ message
        set.setAddress(TX_NODE);
        set.setFrequency(TX_FREC);
        set.setChannel(TX_CH);
        set.setBandwidth(TX_BW);
        set.setCodingRate(TX_CR);
        set.setSpreadingFactor(TX_SF);

        sz = set.serialize(tx_buffer, buf_sz);
      }
        break;

      case CMD_DATA:
      {

    // Create DATA command
    lora::command::Data cmd;

    V_INFO("Create DATA command\n");
    V_INFO("Destination Address: %d\n", dest);
    V_INFO("Message            : %s\n", msg.c_str());
    cmd.setDest(dest);
    cmd.setData(msg);
    sz = cmd.serialize(tx_buffer, buf_sz);

      }
        break;
    }
*/
    // Send command
    V_INFO("Send command\n");
    if (serial.send((const char*) tx_buffer, sz))
    {

      n = 0;
      t = 0;
      bool endPck = false;

      time(&now);
      end = now + 180;
      memset(rx_buffer, 0, buf_sz);

      V_DEBUG("Receive data\n");
      while ((now < end) && !endPck)
      {

        // Receive data
        ssize_t nb = buf_sz - t;
        n = serial.receive((char*) &rx_buffer[t], nb);

        // Process data received
        if (n)
        {
          V_DEBUG("Received %d bytes\n", n);

          if (nb == 0)
          {
            V_DEBUG("Receiver buffer is full\n");
          }
          else
          {
            for (uint16_t i = 0; i < n; i++)
            {
              V_DEBUG("[%d] %x\n", t + i, rx_buffer[t + i]);
              if (rx_buffer[t + i] == 0x04)
              {
                V_DEBUG("Found EOT\n");
                endPck = true;
              }
            }
          /*
           std::cerr << "n: " << std::dec << (int) n << std::endl;
           std::cerr << "t: " << std::dec << (int) t << std::endl;
           */
            t += n;
          /*
           std::cerr << "t: " << std::dec << (int) t << std::endl;
           */
          }
        }
        usleep(100);
        time(&now);
      }

      if (!t)
      {
        std::cerr << "No response received!" << std::endl;
      }
      else
      {
        process_buffer((uint8_t *) rx_buffer, (size_t) t);
      }

    }

  }
  else
  {
    std::cerr << "Error (serial connection): impossible open the serial communication" << std::endl;
  }

  closeSerial(serial);

  return 0;
}

void print_help(void)
{
  std::cerr << "WaspMote Lo-Ra - " << LORA_NAME << " v" << LORA_VERSION << std::endl;
  std::cerr << std::endl;
  std::cerr << "Usage: " << LORA_NAME << " [-v 0|1|2] [-d serial_device] [-b serial_bitrate]"
      << std::endl;
  std::cerr << "       " << LORA_NAME << " -h" << std::endl << std::endl;
  std::cerr << " -b : serial bitrate [1200|2400|4800|9600|19200|38400|57600|115200]. Default value is " << SERIAL_BITRATE << "." << std::endl;
  std::cerr << " -d : serial device. Default value is " << SERIAL_DEVICE << "." << std::endl;
  std::cerr << " -h : display this message." << std::endl;
  std::cerr << " -v : set verbosity level [0|1|2]." << std::endl;


  std::cerr << std::endl;
}



#endif
