//============================================================================
// Name        : main_daemon.cpp
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Main for the "Lo-Ra daemon"
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
#include <pthread.h>

#include "global.h"
#include "verbose.h"
#include "main_daemon.h"
#include "lora/utils.h"
#include "lora/serial.h"
#include "lora/command.h"

//#define LORA_DAEMON

#ifdef LORA_DAEMON

int running = 1;
pthread_mutex_t lock_x;

/*****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
int main_daemon(int argc, char **argv)
{
  int opt = 0;

  uint8_t dest = 0;
  uint8_t timeout = TX_TIMEOUT;
  std::string pipe = PIPE_NAME;
  std::string msg = "";
  std::string device = SERIAL_DEVICE;
  unsigned long bitrate = SERIAL_BITRATE;

  // Serial device handler
  lora::Serial serial;

  // Threads
  pthread_t t_write;
  pthread_t t_read;

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
    //const uint8_t buf_sz = 255;

    //uint8_t rx_buffer[buf_sz] = { 0 };
    Buffer cRxBuffer;
    try
    {
      cRxBuffer.resize(buf_sz);
    }
    catch (std::exception &e)
    {
      throw(e);
      return 0;
    }

    // Empty Rx serial buffer
    rx_buffer_flush(serial);

    tx_param pt;
    pt.timeout = timeout;
    pt.dest = dest;
    pt.error = 0;
    pt.pipe = &pipe;
    pt.serial = &serial;

    rx_param pr;
    pr.error = 0;
    pr.serial = &serial;

    int rc = 0;
    rc = pthread_create(&t_write, NULL, t_write_function, (void *) &pt);
    if (rc)
    {
      perror("Error: impossible create write thread!");
      return 1;
    }

    rc = pthread_create(&t_read, NULL, t_read_function, (void *) &pr);
    if (rc)
    {
      perror("Error: impossible create write thread!");
      return 1;
    }

    pthread_join(t_write, NULL);
    pthread_join(t_read, NULL);

    pthread_cancel(t_write);
    pthread_cancel(t_read);
  }
  else
  {
    std::cerr << "Error (serial connection): impossible open the serial communication" << std::endl;
  }

  closeSerial(serial);

  /* Last thing that main() should do */
  pthread_exit(NULL);

  return 0;
}

void* t_write_function(void *arg)
{
  int pp = 0;
  uint8_t timeout = 0;
  uint8_t dest = 0;
  std::string *pipe = 0;
  lora::Serial *serial = 0;
  tx_param *p = (tx_param*) arg;

  timeout = p->timeout;
  dest = p->dest;
  pipe = p->pipe;
  serial = p->serial;

//  time_t now;
//  time_t end;

  time_t t0;
  time_t t1;

  uint8_t tx_buffer[buf_sz] = { 0 };

//  size_t t = 0;

  Buffer cPipeBuffer;

  V_INFO("Start write treahd!\n");

  t0 = time(&t0);
  t1 = time(&t1);

  try
  {
    cPipeBuffer.resize(buf_sz);
  }
  catch (std::exception &e)
  {
    throw(e);
    p->error = 1;
    return NULL;
  }

  // Prepare command
  ssize_t sz = 0;

  // Check if pipe exists
  if (fileExists(pipe->c_str()))
  {
    V_INFO("Pipe %s exists!\n", pipe->c_str());
  }
  else
  {
    V_INFO("Create pipe %s.\n", pipe->c_str());

    if (mkfifo(pipe->c_str(), 0666) < 0)
    {
      perror("Error: mkfifo( ): ");
      p->error = 1;
      return NULL;
    }
    close(pp);
  }

  V_INFO("Open pipe %s.\n", pipe->c_str());
  pp = open(pipe->c_str(), O_RDONLY, 0);
  if (pp == -1)
  {
    perror("Error: open while opening pipe!");
    p->error = 1;
    return NULL;
  }

  while (running == 1)
  {

    memset(tx_buffer, 0, buf_sz);
    size_t nr = cPipeBuffer.capacity() - cPipeBuffer.size();
    nr = ( nr > (buf_sz - 1) )?( buf_sz - 1 ):nr;

    long int n = read(pp, (void*) tx_buffer, (unsigned long) nr /*(buf_sz - 1)*/ );

    if (n < 0)
    {
      perror("Error: read pipe ");
      p->error = 1;
      return NULL;
    }

    if (n != 0)
    {
/*
      std::cout << "-------------------" << std::endl;
      std::cout << "n: " << n << std::endl;
      std::cout << tx_buffer << std::endl;
      std::cout << "-------------------" << std::endl;
*/
      if (cPipeBuffer.size() == cPipeBuffer.capacity())
      {
        V_DEBUG("Pipe buffer is full. It will be cleaned!\n");
        std::cout << "Buffer full" << std::endl;
        cPipeBuffer.drop(cPipeBuffer.capacity());
      }
      else
      {
        //size_t nw = 
        cPipeBuffer.write(tx_buffer, n);
        //std::cout << "nw: " << nw << std::endl;

        size_t j = 0;
        size_t tot = 0;
        uint8_t buffer[buf_sz] = { 0 };
        memset(buffer, 0, buf_sz);

        for (unsigned int i = 0; i < cPipeBuffer.size(); i++)
        {
          buffer[j] = cPipeBuffer.at(i);

          //std::cout << "i: " << std::dec << i ;
          //std::cout << "     j: " << std::dec << j ;
          //std::cout << "     tot: " << std::dec << tot << std::endl;
          //std::cout << buffer << std::endl;

          if (buffer[j] == '\n')
          {
            buffer[j] = 0;

            std::cout << "Message: " << buffer << std::endl;

            //Create Data Command
            std::string msg = ((char*) buffer);
            uint8_t cmd_buffer[buf_sz] = { 0 };
            sz = createDataCommand(cmd_buffer, dest, msg);

            //std::cout << msg_string(cmd_buffer, sz) << std::endl;

            if (sz)
            {
              // Process Message
              V_INFO("Send command\n");

              t1 = time(&t1);

              while ( t1 < (t0 + timeout) )
              {
                usleep(100);
                t1 = time(&t1);
              }
              //std::cout << "Time: " << (int) t1 - t0 << std::endl;
              t0 = t1;

              pthread_mutex_lock(&lock_x);
              size_t n = serial->send((const char*) cmd_buffer, sz);
              V_INFO("Sent %d bytes.\n", n);
              pthread_mutex_unlock(&lock_x);
            }

            memset(buffer, 0, buf_sz);
            j = 0;
            tot = i;
          }
          else
          {
            buffer[j++] = cPipeBuffer.at(i);

          }

        }

        //std::cout << "Drop " << tot + 1 << std::endl;
        cPipeBuffer.drop(tot + 1);
        //cPipeBuffer.dump();

      }


    }
    usleep(100);
  }

  //close(pp);
  std::cout << std::endl << "exit write" << std::endl;

  return  NULL;
}

void* t_read_function(void *arg)
{
  uint8_t rx_buffer[buf_sz] = { 0 };
  Buffer cRxBuffer;

  lora::Serial *serial = 0;

  V_INFO("Start read treahd!\n");

  rx_param *p = (rx_param*) arg;
  serial = p->serial;

  try
  {
    cRxBuffer.resize(buf_sz);
  }
  catch (std::exception &e)
  {
    throw(e);
    p->error = 1;
    return NULL;
  }

  time_t now;
//  time_t end;

  time_t t0;
  time_t t1;

  size_t t = 0;

  t0 = time(&t0);
  t1 = time(&t1);

  while (running == 1)
  {
    bool endPck = false;

    memset(rx_buffer, 0, buf_sz);
    V_INFO("Waiting response\n");
    while ( !endPck )
    {

      // Receive data
      size_t nr = cRxBuffer.capacity() - cRxBuffer.size();
      nr = ( nr > (buf_sz - 1) )?( buf_sz - 1 ):nr;

      pthread_mutex_lock(&lock_x);
      //long int n = serial.receive((char*) &rx_buffer[t], nr);
      long int n = serial->receive((char*) &rx_buffer[0], (unsigned long) nr /*buf_sz */);
      pthread_mutex_unlock(&lock_x);

      //std::cout << now << "  " << end << std::endl;
      // Process data received
      if ( n > 0 )
      {
        V_DEBUG("Received %d bytes\n", n);

        for (uint16_t i = 0; i < n; i++)
        {
          V_DEBUG("[%d] %x\n", t + i, rx_buffer[i]);
        }

        if (cRxBuffer.size() == cRxBuffer.capacity())
        {
          V_DEBUG("Receiver buffer is full. It will be cleaned!\n");
          cRxBuffer.drop(cRxBuffer.capacity());
        }
        else
        {
/*
          cRxBuffer.write(rx_buffer, n);

          enum _state_0
          {
            LOOKFOR_SOH, LOOKFOR_EOT, CMD_FOUND
          };

          uint8_t state = LOOKFOR_SOH;
          endPck = false;

          for (unsigned int i = 0; i < cRxBuffer.size() && !endPck; i++)
          {
            //V_DEBUG("[%d] %x\n", t + i, rx_buffer[t + i]);
            unsigned long index = i;
            //if (rx_buffer[t + i] == lora::Command::EOT)

            V_DEBUG("State: %d , endPck: %d, [%d] %x\n", state, endPck, index, cRxBuffer.at(index));

            switch (state)
            {
              case LOOKFOR_SOH:
              {
                if (cRxBuffer.at(index) == lora::Command::SOH)
                {
                  V_DEBUG("Found SOH\n");
                  state = LOOKFOR_EOT;
                }

              }
                break;
              case LOOKFOR_EOT:
              {
                if (cRxBuffer.at(index) == lora::Command::EOT)
                {
                  V_DEBUG("Found EOT\n");
                  state = CMD_FOUND;
                  //cRxBuffer.dump(true);
                  endPck = true;
                }

              }
                break;
            }

          }

          t += n;
*/
          memset(rx_buffer, 0, buf_sz);
        }

      }
      usleep(100);
      time(&now);
    }

    if (!t)
    {
      std::cerr << "No response CMD received!" << std::endl;
    }
    else
    {
      unsigned long n = cRxBuffer.read(rx_buffer, t);

      uint8_t err = process_buffer((uint8_t *) rx_buffer, (size_t) t);

      if (err == COM_ERROR)
      {
        // Handle COM_ERROR
        perror("Com error!");
        //running = 0;
      }

      cRxBuffer.drop(n);
    }
    usleep(100);
  }

  //close(pp);
  std::cout << std::endl << "exit read" << std::endl;

  return NULL;
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
      << " -t : minimum time between two send operation. if it is 0 no response are waited. Default value is "
      << TX_TIMEOUT << " seconds" << std::endl;
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

uint8_t createDataCommand(uint8_t *buffer, uint8_t dest, std::string &msg)
{
  // Create DATA command
  lora::command::Data cmd;

  V_INFO("Create DATA command\n");
  V_INFO("Destination Address: %d\n", dest);
  V_INFO("Message            : %s\n", msg.c_str());
  cmd.setDest(dest);
  cmd.setData(msg);

  return cmd.serialize(buffer, buf_sz);

}
#endif
