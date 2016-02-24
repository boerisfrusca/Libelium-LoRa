//============================================================================
// Name        : serial.cpp
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : API to work with a serial device
//============================================================================

#include "serial.h"

#include <iomanip>
#include <sstream>
#include <string.h>
#include <stdio.h>

namespace lora
{
  /*************************************************************************
   * class Serial::Exception
   ************************************************************************/
  Serial::Exception::~Exception() throw ()
  {
    this->~exception();
  }

  const char* Serial::Exception::what() const throw ()
  {
    std::string msg;
    switch (m_value)
    {
      case INVALID_BITRATE:
        return "Invalid bit-rate";
        break;

      case INVALID_DEVICE:
        return "Invalid serial device name";
        break;

      case OPEN_DEVICE_FAILURE:
        return "Error: can't open device";
        break;

      case CLOSE_DEVICE_FAILURE:
        return "Error: can't close device";
        break;

      default:
        break;
    }
    return "General exception";
  }

  /*************************************************************************
   * class Serial
   ************************************************************************/
  const std::string Serial::DEFAULT_DEVICE = "/dev/USB0";
  const unsigned int Serial::DEFAULT_BITRATE = 9600;

  Serial::Serial() :
      m_fd(-1)
  {
    m_device = DEFAULT_DEVICE;
    m_bitrate = DEFAULT_BITRATE;
  }

  Serial::Serial(std::string device, unsigned int bitrate) throw (Exception) :
      m_fd(-1)
  {
    m_device = device;
    setBitrate(bitrate);
  }

  Serial::~Serial()
  {
    if (m_fd >= 0)
    {
      closeDev();
    }
  }

  void Serial::setBitrate(std::string bitrate) throw (Exception)
  {
    unsigned long br = 0;

    std::stringstream(bitrate) >> br;
    setBitrate(br);
  }

  unsigned long Serial::bitrate() const
  {
    switch (m_bitrate)
    {
      case B75:
      {
        return 75;
      }
        break;

      case B110:
      {
        return 110;
      }
        break;

      case B300:
      {
        return 300;
      }
        break;

      case B1200:
      {
        return 1200;
      }
        break;

      case B2400:
      {
        return 2400;
      }
        break;

      case B4800:
      {
        return 4800;
      }
        break;

      case B9600:
      {
        return 9600;
      }
        break;

      case B19200:
      {
        return 19200;
      }
        break;

      case B38400:
      {
        return 38400;
      }
        break;

      case B57600:
      {
        return 57600;
      }
        break;

      case B115200:
      {
        return 115200;
      }
        break;
      default:
      {
        throw Exception(Exception::INVALID_BITRATE);
      }
    }
  }

  void Serial::setBitrate(unsigned long bitrate) throw (Exception)
  {
    switch (bitrate)
    {
      case 75:
      {
        m_bitrate = B75;
      }
        ;
        break;

      case 110:
      {
        m_bitrate = B110;
      }
        ;
        break;

      case 300:
      {
        m_bitrate = B300;
      }
        ;
        break;

      case 1200:
      {
        m_bitrate = B1200;
      }
        ;
        break;

      case 2400:
      {
        m_bitrate = B2400;
      }
        ;
        break;

      case 4800:
      {
        m_bitrate = B4800;
      }
        ;
        break;

      case 9600:
      {
        m_bitrate = B9600;
      }
        ;
        break;

      case 19200:
      {
        m_bitrate = B9600;
      }
        ;
        break;

      case 38400:
      {
        m_bitrate = B38400;
      }
        ;
        break;

      case 57600:
      {
        m_bitrate = B57600;
      }
        ;
        break;

      case 115200:
      {
        m_bitrate = B115200;
      }
        break;
      default:
      {
        throw Exception(Exception::INVALID_BITRATE);
      }
    }
  }

  void Serial::openDev() throw (Exception)
  {
    m_fd = open(m_device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (m_fd == -1)
    {
      /*
       * Could not open the port.
       */
      throw Exception(Exception::OPEN_DEVICE_FAILURE);
    }
    else
    {
      int n = fcntl(m_fd, F_GETFL, 0);
      fcntl(m_fd, F_SETFL, n & ~O_NDELAY);

      setInterfaceAttribs(0);
    }
  }

  void Serial::closeDev() throw (Exception)
  {
    /* restore the old port settings */
    tcsetattr(m_fd, TCSANOW, &m_oldtio);
    close(m_fd);
  }

  int Serial::setInterfaceAttribs(int parity)
  {
    // save current serial port settings
    tcgetattr(m_fd, &m_oldtio);
    // clear struct for new port settings
    bzero(&m_newtio, sizeof(m_newtio));

    memset(&m_newtio, 0, sizeof m_newtio);
    if (tcgetattr(m_fd, &m_newtio) != 0)
    {
      printf("error %d from tcgetattr", errno);
      return -1;
    }

    cfsetospeed(&m_newtio, m_bitrate);
    cfsetispeed(&m_newtio, m_bitrate);

    m_newtio.c_cflag = (m_newtio.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    m_newtio.c_iflag &= ~IGNBRK;                 // ignore break signal
    m_newtio.c_lflag = 0;                        // no signaling chars, no echo,
                                                 // no canonical processing
    m_newtio.c_oflag = 0;                        // no remapping, no delays

    m_newtio.c_cc[VMIN] = 0;                     // read doesn't block
    m_newtio.c_cc[VTIME] = 5;                    // 0.5 seconds read timeout

    m_newtio.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    m_newtio.c_cflag |= (CLOCAL | CREAD);        // ignore modem controls,
                                                 // enable reading
    m_newtio.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    m_newtio.c_cflag |= parity;
    m_newtio.c_cflag &= ~CSTOPB;
    m_newtio.c_cflag &= ~CRTSCTS;

    m_newtio.c_iflag &= ~ICRNL;                  // Map CR to NL

    // now clean the modem line and activate the settings for the port
    tcflush(m_fd, TCIOFLUSH);

    if (tcsetattr(m_fd, TCSANOW, &m_newtio) != 0)
    {
      printf("error %d from tcsetattr", errno);
      return -1;
    }
    return 0;
  }

  ssize_t Serial::receive(const char* buffer, ssize_t size)
  {
    int n = read(m_fd, (char*) buffer, size);

    return n;
  }

  ssize_t Serial::send(const char* buffer, ssize_t size)
  {
    int n = write(m_fd, (char*) buffer, size);

    return n;
  }

  void Serial::dump()
  {
    std::cerr << "\t" << std::left << std::setw(10) << std::setfill(' ') << "Device" << ": "
        << m_device << std::endl;
    std::cerr << "\t" << std::left << std::setw(10) << std::setfill(' ') << "Bitrate" << ": "
        << (unsigned long) bitrate() << std::endl;
  }

} /* namespace t2 */
