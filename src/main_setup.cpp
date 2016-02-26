//============================================================================
// Name        : main_sender.cpp
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Main for the "Lo-Ra configuration writer"
//============================================================================
#include <iostream>
#include <string>
#include <cstring>
#include <stdarg.h>
#include <stdlib.h>
#include "global.h"
#include "verbose.h"
#include "main_setup.h"
#include "lora/utils.h"
#include "lora/serial.h"
#include "lora/command.h"

#ifdef LORA_SETUP
/*****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
int main_setup(int argc, char **argv)
{
  int opt = 0;
  std::string msg;
  std::string device = SERIAL_DEVICE;
  unsigned long bitrate = SERIAL_BITRATE;

  uint8_t addr = TX_NODE;
  uint8_t ch   = TX_CH;
  uint8_t fr   = TX_FREC;
  uint8_t cr   = TX_CR;
  uint8_t sf   = TX_SF;
  uint8_t bw   = TX_BW;

  // Serial device handler
  lora::Serial serial;

  if ( argc == 1)
  {
    print_help();
    return 1;
  }

  // Parse command line
  while ((opt = getopt(argc, argv, "v:a:b:c:d:f:r:s:w:h")) != -1)
  {
    switch (opt)
    {
      // Address
      case 'a':
      {
        bool error = false;
        if (is_number(optarg))
        {
          int n = 0;
          std::stringstream ss(optarg);
          ss >> n;

          if (n < 1 || n > 255)
          {
            error = true;
          }
          else
          {
            addr = (uint8_t) (n & 0x0FF);
          }

        }
        else
        {
          error = true;
        }

        if (error)
        {
          std::cerr << "Error: destination address must be a number between 1 and 255."
              << std::endl;
          std::cerr << "Type '" << LORA_NAME << " -h' for help." << std::endl;
          return 0;
        }
      }
        break;

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

        // Channel
      case 'c':
      {
        bool error = false;
        if (is_number(optarg))
        {
          int n = 0;
          std::stringstream ss(optarg);
          ss >> n;

          if (n < 0 || n > 17)
          {
            error = true;
          }
          else
          {
            switch (n)
            {
              case 0:
                ch = lora::ConfigCommand::CH_00;
                break;
              case 1:
                ch = lora::ConfigCommand::CH_01;
                break;
              case 2:
                ch = lora::ConfigCommand::CH_02;
                break;
              case 3:
                ch = lora::ConfigCommand::CH_03;
                break;
              case 4:
                ch = lora::ConfigCommand::CH_04;
                break;
              case 5:
                ch = lora::ConfigCommand::CH_05;
                break;
              case 6:
                ch = lora::ConfigCommand::CH_06;
                break;
              case 7:
                ch = lora::ConfigCommand::CH_07;
                break;
              case 8:
                ch = lora::ConfigCommand::CH_08;
                break;
              case 9:
                ch = lora::ConfigCommand::CH_09;
                break;
              case 10:
                ch = lora::ConfigCommand::CH_10;
                break;
              case 11:
                ch = lora::ConfigCommand::CH_11;
                break;
              case 12:
                ch = lora::ConfigCommand::CH_12;
                break;
              case 13:
                ch = lora::ConfigCommand::CH_13;
                break;
              case 14:
                ch = lora::ConfigCommand::CH_14;
                break;
              case 15:
                ch = lora::ConfigCommand::CH_15;
                break;
              case 16:
                ch = lora::ConfigCommand::CH_16;
                break;
              case 17:
                ch = lora::ConfigCommand::CH_17;
                break;
              default:
                ch = lora::ConfigCommand::CH_00;
                break;
            }
          }

        }
        else
        {
          error = true;
        }

        if (error)
        {
          std::cerr << "Error: channel must between 0 and 17." << std::endl;
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

        // Frequency band
      case 'f':
      {
        bool error = false;
        if (is_number(optarg))
        {
          int n = 0;
          std::stringstream ss(optarg);
          ss >> n;

          if (n != 868 && n != 900)
          {
            error = true;
          }
          else
          {
            switch (n)
            {
              case 868:
                fr = lora::ConfigCommand::F_868;
                break;
              case 900:
                fr = lora::ConfigCommand::F_900;
                break;
              default:
                fr = lora::ConfigCommand::F_868;
                break;
            }
          }

        }
        else
        {
          error = true;
        }

        if (error)
        {
          std::cerr << "Error: frequency must be 868 or 900." << std::endl;
          std::cerr << "Type '" << LORA_NAME << " -h' for help." << std::endl;
          return 0;
        }
      }
        break;

        // Print help
      case 'h':
        print_help();
        return 1;

        // Coding rate
      case 'r':
      {
        bool error = false;
        if (is_number(optarg))
        {
          int n = 0;
          std::stringstream ss(optarg);
          ss >> n;

          if (n < 5 || n > 8)
          {
            error = true;
          }
          else
          {
            switch (n)
            {
              case 5:
                cr = lora::ConfigCommand::CR_5;
                break;
              case 6:
                cr = lora::ConfigCommand::CR_6;
                break;
              case 7:
                cr = lora::ConfigCommand::CR_7;
                break;
              case 8:
                cr = lora::ConfigCommand::CR_8;
                break;
              default:
                cr = lora::ConfigCommand::CR_5;
                break;
            }
          }

        }
        else
        {
          error = true;
        }

        if (error)
        {
          std::cerr << "Error: coding rate must be must between 5 and 8." << std::endl;
          std::cerr << "Type '" << LORA_NAME << " -h' for help." << std::endl;
          return 0;
        }
      }
        break;

        // Spreading factor
      case 's':
      {
        bool error = false;
        if (is_number(optarg))
        {
          int n = 0;
          std::stringstream ss(optarg);
          ss >> n;

          if (n < 6 || n > 12)
          {
            error = true;
          }
          else
          {
            switch (n)
            {
              case 6:
                sf = lora::ConfigCommand::SF_6;
                break;
              case 7:
                sf = lora::ConfigCommand::SF_7;
                break;
              case 8:
                sf = lora::ConfigCommand::SF_8;
                break;
              case 9:
                sf = lora::ConfigCommand::SF_9;
                break;
              case 10:
                sf = lora::ConfigCommand::SF_10;
                break;
              case 11:
                sf = lora::ConfigCommand::SF_11;
                break;
              case 12:
                sf = lora::ConfigCommand::SF_12;
                break;
              default:
                sf = lora::ConfigCommand::SF_6;
                break;
            }
          }

        }
        else
        {
          error = true;
        }

        if (error)
        {
          std::cerr << "Error: spreading factor must between 6 and 12." << std::endl;
          std::cerr << "Type '" << LORA_NAME << " -h' for help." << std::endl;
          return 0;
        }
      }
        break;

      case 'v':
        // Verbose level
        v_verbosity(atoi(optarg));
        break;

      // Bandwidth
      case 'w':
      {
        bool error = false;
        if (is_number(optarg))
        {
          int n = 0;
          std::stringstream ss(optarg);
          ss >> n;

          if ( n != 125 && n != 250 && n != 500 )
          {
            error = true;
          }
          else
          {
            switch (n)
            {
              case 125:
                bw = lora::ConfigCommand::BW_125;
                break;
              case 250:
                bw = lora::ConfigCommand::BW_250;
                break;
              case 500:
                bw = lora::ConfigCommand::BW_500;
                break;
              default:
                bw = lora::ConfigCommand::BW_125;
                break;
            }
          }

        }
        else
        {
          error = true;
        }

        if (error)
        {
          std::cerr << "Error: bandwidth must be 125, 250 or 500 KHz." << std::endl;
          std::cerr << "Type '" << LORA_NAME << " -h' for help." << std::endl;
          return 0;
        }
      }
        break;

      default:
        std::cerr << "Type '" << LORA_NAME << "-h' for help." << std::endl;
        std::cerr << std::endl;
        return 0;
    }
  }

  // Check frequency and channel
  if ( ( fr == lora::ConfigCommand::F_868 && ch < lora::ConfigCommand::CH_10 ) ||
       ( fr == lora::ConfigCommand::F_900 && ch > lora::ConfigCommand::CH_12 ) )
  {
    std::cerr << "Error: channel allowed are 10 to 17 for 868 MHz band and 0 to 12 for 900 MHz band." << std::endl;
    std::cerr << "Type '" << LORA_NAME << " -h' for help." << std::endl;
    return 0;
  }

  V_DEBUG ("Address          : %d\n", (int) addr);
  V_DEBUG ("Frequency code   : %d\n", (int) fr);
  V_DEBUG ("Channel code     : %d\n", (int) ch);
  V_DEBUG ("Bandwidth code   : %d\n", (int) bw);
  V_DEBUG ("CR code          : %d\n", (int) cr);
  V_DEBUG ("SF code          : %d\n", (int) sf);


  V_DEBUG("Serial device : %s\n", device.c_str());
  V_DEBUG("Serial bitrate: %ld\n", bitrate);

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
    rx_buffer_flush(serial);

    // Prepare command
    ssize_t sz = 0;

    // Create SET message
    V_INFO("Create SET command\n");
    lora::command::Set cmd;

    cmd.setAddress(addr);
    cmd.setFrequency(fr);
    cmd.setChannel(ch);
    cmd.setBandwidth(bw);
    cmd.setCodingRate(cr);
    cmd.setSpreadingFactor(sf);

    sz = cmd.serialize(tx_buffer, buf_sz);

    V_DEBUG("SET COMMAND: %s\n", msg_string(tx_buffer, sz).c_str());

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
             t += n;
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
  std::cerr << "Usage: " << LORA_NAME << " [-v 0|1|2] [-d serial_device] [-b serial_bitrate] [-a address] [-f frequency] [-c channel]";
  std::cerr << " [-w bandwidth] [-r coding_rate] [-s spreading_factor]"
      << std::endl;
  std::cerr << "       " << LORA_NAME << " -h" << std::endl << std::endl;
  std::cerr
      << " -a : node address. It must be a number between 1 and 255. Default value is 0 (broadcast)"
      << std::endl;
  std::cerr
      << " -b : serial bitrate [1200|2400|4800|9600|19200|38400|57600|115200]. Default value is "
      << SERIAL_BITRATE << "." << std::endl;
  std::cerr
      << " -c : channel. Channel allowed are 1' to 17 for 868 MHz band and 0 to 12 for 900 MHz band. Default channel id 10."
      << std::endl;
  std::cerr << " -d : serial device. Default value is " << SERIAL_DEVICE << "." << std::endl;
  std::cerr << " -f : frequency band. Bands allowed are 900 and 868 MHz. Default value is 868."
      << std::endl;
  std::cerr << " -r : coding rate. It must be a number between 5 and 8. Default value is 5."
      << std::endl;
  std::cerr << " -s : spreading factor. It must be a number between 6 and 12. Default value is 6."
      << std::endl;
  std::cerr << " -v : set verbosity level [0|1|2] ." << std::endl;
  std::cerr << " -w : bandwidth. Allowed values are 125, 250 and 500 MHz. Default value is 125."
      << std::endl;

  std::cerr << std::endl;
}

#endif

