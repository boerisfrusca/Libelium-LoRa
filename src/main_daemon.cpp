//============================================================================
// Name        : main_sender.cpp
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Main for the "Lo-Ra sender"
//============================================================================

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>
#include <cstring>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "global.h"
#include "verbose.h"
#include "main_daemon.h"
#include "lora/utils.h"
#include "lora/serial.h"
#include "lora/command.h"

#ifdef LORA_DAEMON

int running = 1;

/*****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
int main_daemon(int argc, char **argv)
{
  int opt = 0;
  int pp = 0;

  uint8_t dest = 0;
  uint8_t timeout = RX_TIMEOUT;
  std::string pipe = PIPE_NAME;
  std::string msg = "";
  std::string device = SERIAL_DEVICE;
  unsigned long bitrate = SERIAL_BITRATE;

  // Serial device handler
  lora::Serial serial;

  running = 1;

  // Try to catch CTRL-C signal and calling the corresponding routine
  signal(SIGINT, signalCallbackHandler);

  if (argc == 1)
  {
    print_help();
    return 1;
  }

  // Parse command line
  while ((opt = getopt(argc, argv, "v:a:b:d:p:t:")) != -1)
  {
    switch (opt)
    {
      // Destination address
      case 'a':
      {
        bool error = false;
        if (is_number(optarg))
        {
          int n = 0;
          std::stringstream ss(optarg);
          ss >> n;

          if (n < 0 || n > 255)
          {
            error = true;
          }
          else
          {
            dest = (uint8_t) (n & 0x0FF);
          }

        }
        else
        {
          error = true;
        }

        if (error)
        {
          std::cerr
              << "Error: destination address must be a number between 1 and 255 or 0 for broadcast."
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

        // Message
      case 'p':
      {

        pipe = optarg;
      }
        break;

        // Timeout
      case 't':
      {
        bool error = false;
        if (is_number(optarg))
        {
          int n = 0;
          std::stringstream ss(optarg);
          ss >> n;

          if (n < 0 || n > 255)
          {
            error = true;
          }
          else
          {
            timeout = (uint8_t) (n & 0x0FF);
          }

        }
        else
        {
          error = true;
        }

        if (error)
        {
          std::cerr << "Error: Maximum timeout supported is 255." << std::endl;
          std::cerr << "Type '" << LORA_NAME << " -h' for help." << std::endl;
          return 0;
        }
      }
        break;

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

    const uint8_t buf_sz = 255;

    uint8_t tx_buffer[buf_sz] = { 0 };
    uint8_t rx_buffer[buf_sz] = { 0 };

    // Empty Rx serial buffer
    rx_buffer_flush(serial);

    // Prepare command
    ssize_t sz = 0;

    // Check if pipe exists
    if (fileExists(pipe.c_str()))
    {
      V_INFO("Pipe %s exists!\n", pipe.c_str());
    }
    else
    {
      V_INFO("Create pipe %s.\n", pipe.c_str());

      if (mkfifo(pipe.c_str(), 0666) < 0)
      {
        perror("Error: mkfifo( ): ");
        return 1;
      }
      close(pp);
    }

    V_INFO("Open pipe %s.\n", pipe.c_str());
    pp = open(pipe.c_str(), O_RDONLY, 0);
    if (pp == -1)
    {
      perror("Error: open while opening pipe!");
      return 0;
    }

    while (running == 1)
    {

      memset(tx_buffer, 0, buf_sz);
      long int n = read(pp, (void*) tx_buffer, (unsigned long) buf_sz);

      if (n < 0)
      {
        perror("Error: read( ): ");
        return 1;
      }

      if (n != 0)
      {
        char *sentence = (char*) tx_buffer;

        std::stringstream ss((const char*) sentence);

        if (sentence != NULL)
        {
          while (std::getline(ss, msg, '\n'))
          {
            std::cout << msg << std::endl;



            // Create DATA command
            lora::command::Data cmd;

            V_INFO("Create DATA command\n");
            V_INFO("Destination Address: %d\n", dest);
            V_INFO("Message            : %s\n", msg.c_str());
            cmd.setDest(dest);
            cmd.setData(msg);
            sz = cmd.serialize(tx_buffer, buf_sz);

            V_INFO("Send command\n");
            if (serial.send((const char*) tx_buffer, sz))
            {

              if (timeout)
              {
                n = 0;
                t = 0;
                bool endPck = false;

                time(&now);
                end = now + timeout;
                memset(rx_buffer, 0, buf_sz);

                V_INFO("Waiting response\n");
                while ((now < end) && !endPck)
                {

                  // Receive data
                  ssize_t nb = buf_sz - t;
                  n = serial.receive((char*) &rx_buffer[t], nb);

                  //std::cout << now << "  " << end << std::endl;
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


          }
        }


      }
      usleep(100);
    }

    close(pp);
    std::cout << std::endl << "exit" << std::endl;

    return 1;


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
  std::cerr << "Usage: " << LORA_NAME
      << " [-v 0|1|2] [-d serial_device] [-b serial_bitrate] [-a [0-255]] [-p <pipe-path>] [-t timeout]"
      << std::endl;
  std::cerr << "       " << LORA_NAME << " -h" << std::endl << std::endl;

  std::cerr
      << " -a : destination address. It must be a number between 1 and 255, 0 is for broadcast message. Default value is 0 (broadcast)"
      << std::endl;
  std::cerr
      << " -b : serial bitrate [1200|2400|4800|9600|19200|38400|57600|115200]. Default value is "
      << SERIAL_BITRATE << "." << std::endl;
  std::cerr << " -d : serial device. Default value is " << SERIAL_DEVICE << "." << std::endl;
  std::cerr << " -h : display this message." << std::endl;
  std::cerr << " -p : pipe used for receiving data to send. Default value is " << PIPE_NAME << "."
      << std::endl;
  std::cerr
      << " -t : timeout to wait response in seconds. if it is 0 no response are waited. Default value is "
      << RX_TIMEOUT << " seconds" << std::endl;
  std::cerr << " -v : set verbosity level  [0|1|2]." << std::endl;

  std::cerr << std::endl;
}

bool fileExists(const char* file)
{
  struct stat buf;
  return (stat(file, &buf) == 0);
}

void signalCallbackHandler(int signum)
{
  // Set global running flag to 0 (terminate reading loop)
  V_INFO("Signal %d\n", signum);
  running = 0;
  exit(0);
}

#endif
