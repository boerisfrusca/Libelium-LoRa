//============================================================================
// Name        : command.h
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Lo-Ra commands
//============================================================================
#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stdint.h>
#include <vector>
#include <iostream>
#include "interfaces.h"

#define DEBUG 0

namespace lora
{
  namespace command
  {
    class Read: public Command, public OutputCommand
    {
      public:

        /// Size of the command field: 4 bytes (ASCII string).
        static const uint8_t SZ_CMD = 4;

        /// Size of the payload: only the command
        static const uint8_t SZ_PAYLOAD = SZ_CMD;

        /**
         * Creates a READ LoRa message for WaspMote.
         *
         */
        Read();

        /**
         * Copy constructor
         *
         * \param cmd command source
         */
        Read(const Read &cmd);

        /**
         * Destroy a serial device object.
         *
         */
        virtual ~Read();

        /**
         * Serialize a message
         *
         * \param buffer array if byte where store he message
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte of the message. 0 if there was an error.
         */
        virtual uint8_t serialize(uint8_t *buffer, size_t size);

      protected:
        /**
         * Creates field type
         *
         * \param buffer array if byte where store he message
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte written into the buffer. 0 if there was an error.
         */
        virtual uint8_t createFieldType(uint8_t *buffer, uint8_t size);

    };

    class Info: public ConfigCommand, public InputCommand
    {
      public:

        /// Size of the command field: 4 bytes (ASCII string).
        static const uint8_t SZ_CMD = 4;

        /// Size of the payload: only the command
        static const uint8_t SZ_PAYLOAD = 0;

        /**
         * Creates a INFO LoRa message for WaspMote.
         *
         */
        Info();

        /**
         * Copy constructor
         *
         * \param cmd command source
         */
        Info(const Info &cmd);

        /**
         * Destroy a serial device object.
         *
         */
        virtual ~Info();

        /**
         * Creates from a buffer
         *
         * \param buffer array where message is stored
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte processed from the buffer. 0 if there was an error.
         */
        virtual uint8_t createFromBuffer(uint8_t *buffer, size_t size);

      protected:
        /**
         * Creates field type
         *
         * \param buffer array if byte where store he message
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte written into the buffer. 0 if there was an error.
         */
        virtual uint8_t createFieldType(uint8_t *buffer, uint8_t size);

    };

    class Set: public ConfigCommand, public OutputCommand
    {
      public:

        /// Size of the command field: 4 bytes (ASCII string).
        static const uint8_t SZ_CMD = 3;

        /// Size of the payload: only the command
        static const uint8_t SZ_PAYLOAD = 0;

        /**
         * Creates a SET LoRa message for WaspMote.
         *
         */
        Set();

        /**
         * Copy constructor
         *
         * \param msg message source
         */
        Set(const Set &msg);

        /**
         * Destroy a serial device object.
         *
         */
        virtual ~Set();

        /**
         * Serialize a message
         *
         * \param buffer array if byte where store he message
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte of the message. 0 if there was an error.
         */
        virtual uint8_t serialize(uint8_t *buffer, size_t size);

      protected:
        /**
         * Creates field type
         *
         * \param buffer array if byte where store he message
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte written into the buffer. 0 if there was an error.
         */
        virtual uint8_t createFieldType(uint8_t *buffer, uint8_t size);

        /**
         * Creates the payload of the message.
         *
         * \param buffer array where message payload is stored
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte processed from the buffer. 0 if there was an error.
         */
        virtual uint8_t createPayload(uint8_t *buffer, size_t size);

    };

    class Error: public lora::Command, public InputCommand
    {
      public:

        /// Size of the command field: 4 bytes (ASCII string).
        static const uint8_t SZ_CMD = 5;

        /// Size of the payload: only the command
        static const uint8_t SZ_PAYLOAD = 0;

        /**
         * Creates a INFO LoRa message for WaspMote.
         *
         */
        Error();

        /**
         * Copy constructor
         *
         * \param cmd command source
         */
        Error(const Error &cmd);

        /**
         * Destroy a serial device object.
         *
         */
        virtual ~Error();

        /**
         * Creates message from buffer.
         *
         * \param buffer array where message payload is stored
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte processed from the buffer. 0 if there was an error.
         */
        virtual uint8_t createFromBuffer(uint8_t *buffer, size_t size);

        /**
         * Gets the error message.
         *
         * \return the message error.
         */
        const std::string& error()
        {
          return m_error;
        }
        ;

      protected:
        /**
         * Creates field type
         *
         * \param buffer array if byte where store he message
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte written into the buffer. 0 if there was an error.
         */
        virtual uint8_t createFieldType(uint8_t *buffer, uint8_t size);

        //! Error message
        std::string m_error;

    };

    class Data: public lora::Command, public OutputCommand
    {
      public:

        /// Size of the command field: 4 bytes (ASCII string).
        static const uint8_t SZ_CMD = 4;

        /// Size of the payload: only the command
        static const uint8_t SZ_PAYLOAD = 0;

        /**
         * Creates a INFO LoRa message for WaspMote.
         *
         */
        Data();

        /**
         * Copy constructor
         *
         * \param msg message source
         */
        Data(const Data &msg);

        /**
         * Destroy a serial device object.
         *
         */
        virtual ~Data();

        /**
         * Serialize a message
         *
         * \param buffer array if byte where store he message
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte of the message. 0 if there was an error.
         */
        virtual uint8_t serialize(uint8_t *buffer, size_t size);

        void setDest(uint8_t dst)
        {
          m_dest = dst;
        }
        ;

        uint8_t dest()
        {
          return m_dest;
        }

        void setData(std::string& data)
        {
          m_data = data;
        }
        ;

        const std::string& data()
        {
          return m_data;
        }
        ;

      protected:
        /**
         * Creates field type
         *
         * \param buffer array if byte where store he message
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte written into the buffer. 0 if there was an error.
         */
        virtual uint8_t createFieldType(uint8_t *buffer, uint8_t size);

        /**
         * Creates the payload of the message.
         *
         * \param buffer array where message payload is stored
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte processed from the buffer. 0 if there was an error.
         */
        virtual uint8_t createPayload(uint8_t *buffer, size_t size);

        //! Destination address
        uint8_t m_dest;

        //! Data to sent
        std::string m_data;

    };

    class Ack: public lora::Command, public InputCommand
    {
      public:

        /// Size of the command field: 4 bytes (ASCII string).
        static const uint8_t SZ_CMD = 3;

        /// Size of the payload: only the command
        static const uint8_t SZ_PAYLOAD = 0;

        /**
         * Creates a INFO LoRa message for WaspMote.
         *
         */
        Ack();

        /**
         * Copy constructor
         *
         * \param cmd command source
         */
        Ack(const Ack &cmd);

        /**
         * Destroy a serial device object.
         *
         */
        virtual ~Ack();

        /**
         * Creates message from buffer.
         *
         * \param buffer array where message payload is stored
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte processed from the buffer. 0 if there was an error.
         */
        virtual uint8_t createFromBuffer(uint8_t *buffer, size_t size);

      protected:
        /**
         * Creates field type
         *
         * \param buffer array if byte where store he message
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte written into the buffer. 0 if there was an error.
         */
        virtual uint8_t createFieldType(uint8_t *buffer, uint8_t size);

        /**
         * Creates the payload of the message.
         *
         * \param buffer array where message payload is stored
         * \param size size of the buffer in number of bytes
         *
         * \return number of byte processed from the buffer. 0 if there was an error.
         */
        virtual uint8_t createPayload(uint8_t *buffer, size_t size);

    };

  } /* namespace command */

} /* namespace lora */

#endif /* MESSAGE_H_ */
