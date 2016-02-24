//============================================================================
// Name        : serial.h
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : API to work with a serial device
//============================================================================
#ifndef _LORA_SERIAL_H_
#define _LORA_SERIAL_H_

#include <iostream>
#include <exception>    // using standard exceptions

#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

namespace lora
{
  /**
   * @brief The Serial class provides some common APIs to work with a serial
   * device.
   *
   * A serial device is a interface through which information are transfered
   * one bit at a time. The transfer speed is measured in bit per second
   * (bps). The operative system sees a device as a file in the '/dev'
   * directory.
   *
   */
  class Serial
  {
      //! serial device name
      std::string m_device;

      //! serial baud rate
      unsigned int m_bitrate;

      //! serial file descriptor
      int m_fd;

      //! Previous serial settings (before having opened the serial device)
      struct termios m_oldtio;

      //! New serial settings (after having opened the serial device)
      struct termios m_newtio;

      int setInterfaceAttribs(int parity);

    public:

      /**
       * @brief Custom class to handles exception for the Serial class.
       *
       */
      class Exception: public std::exception
      {
          //! Exception value
          unsigned int m_value;

        public:
          /**
           * @brief Exception codes for the serial class.
           */
          enum ExceptionCode
          {
            /// Generic exception
            UNKNOWN_EXCEPTION = 0,

            /// Invalid baud rate value
            INVALID_BITRATE,

            /// Invalid serial device
            INVALID_DEVICE,

            /// Open device error
            OPEN_DEVICE_FAILURE,

            /// Close device error
            CLOSE_DEVICE_FAILURE,
          };

          /**
           * @brief Constructs an exception.
           *
           */
          Exception()
          {
            m_value = 0;
          }
          ;

          /**
           * @brief Constructor, it sets exception code.
           *
           * @param[in] code exception code.
           */
          Exception(int code)
          {
            m_value = code;
          }
          ;

          /**
           * @brief Destroy the exception object.
           *
           */
          virtual ~Exception() throw ();

          /**
           * @brief Returns an explanatory string.
           *
           * @returns explanatory string.
           */
          virtual const char* what() const throw ();
      };

      /// Default device name
      static const std::string DEFAULT_DEVICE;

      /// Default bitrate
      static const unsigned int DEFAULT_BITRATE;

      /**
       * @brief Creates a serial device object.
       *
       */
      Serial();

      /**
       * @brief Creates a serial device object.
       *
       * This function initializes the serial port name with the values ​​
       * "device name" and speed to "bitrate" value.
       *
       * @param[in] device serial device name
       * @param[in] bitrate baud rate. It must be a number among 75, 110, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600 and 115200.
       *
       */
      Serial(std::string device, unsigned int bitrate = DEFAULT_BITRATE) throw (Exception);

      /**
       * @brief Destroy a serial device object..
       *
       */
      virtual ~Serial();

      /**
       * @brief Returns the device name.
       *
       * @returns device name.
       */
      const std::string& device() const
      {
        return m_device;
      }

      /**
       * @brief Returns the bit-rate code: enum B1200, B2400, ..., B115200.
       *
       * @returns bit-rate code.
       */
      const unsigned int bitrateCode() const
      {
        return m_bitrate;
      }

      /**
       * Gets bit-rate as spped in bps (bit-per-seconds)
       *
       * \return speed as bps
       */
      unsigned long bitrate() const;

      /**
       * @brief Sets bit-rate.
       *
       * This function require bitrate value as speed in bps. If this value
       * is not a standard serial speed an exception is thrown.
       *
       * @param[in] bitrate bit-rate. It must be a number among 75, 110, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600 and 115200.
       */
      void setBitrate(unsigned long bitrate) throw (Exception);

      /**
       * @brief Sets bit-rate.
       *
       * This function require bitrate value as speed in bps. If this value
       * is not a standard serial speed an exception is thrown.
       *
       * @param[in] bitrate bit-rate. It must be a string with a number among 75, 110, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600 and 115200.
       */
      void setBitrate(std::string bitrate) throw (Exception);

      /**
       * @brief Sets the device name.
       *
       * The device name is the path of file which the operative system see
       * the physical device.
       *
       * @param[in] device device name as a path.
       */
      void setDevice(std::string device)
      {
        m_device = device;
      }
      ;

      /**
       * @brief Opens a connection with a serial device.
       *
       * This function ppens a serial device. If there is an error an exception
       * is thrown.
       *
       */
      void openDev() throw (Exception);

      /**
       * @brief Closes a connection with a serial device.
       *
       * This function closes a serial device. If there is an error an exception
       * is thrown.
       *
       */
      void closeDev() throw (Exception);

      /**
       * @brief Receives bytes from a serial device.
       *
       * This function reads bytes from a serial device and returns the number of byte
       * read.
       * \bNote: pay attention to use a buffer with at least 'size' elements.
       *
       * @param[in] buffer buffer where received bytes are saved.
       * @param[in] size buffer size.
       *
       * @returns  the number of byte read.
       */
      ssize_t receive(const char* buffer, ssize_t size);

      /**
       * @brief Sends bytes to a serial device.
       *
       * This function writes bytes to a serial device and returns the number
       * of byte written.
       *
       * @param[in] buffer buffer with bytes to send.
       * @param[in] size buffer size.
       *
       * @returns  the number of byte written.
       */
      ssize_t send(const char* buffer, ssize_t size);

      /**
        * @brief Prints on standard error main information of the serial device state.
        *
        */
      void dump();
  };

} /* namespace lora */
#endif /* _LORA_SERIAL_H_ */
