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
    /**
     * @brief Class to describe a READ command.
     *
     * The READ command in Libelium SX1272 LoRa protocol is used for receiving
     * the current configuration of the LoRa module. This class implements the
     * Command and OutputCommand interfaces.
     *
     * The READ command has this structure:
     *
     *   - SOH character (Start of Header – ASCII 0×01)
     *   - command: READ
     *   - CR+LF characters (Carry return + line feed -ASCII 0x0D and 0x0A).
     *   - CRC16-Modbus code of the bytes between SOH and CR+LF (not including these)
     *   - EOT character (End of Transmission – ASCII 0×04)
     *
     * The frame would be:
     * [SOH]READ[CR+LF]2A31[EOT]
     *
     * The same command in hexadecimal format: 01 52 45 41 44 0D 0A 32 41 33 31 04
     *
     */
    class Read: public Command, public OutputCommand
    {
      public:

        /// Size of the command field: 4 bytes (ASCII string).
        static const uint8_t SZ_CMD = 4;

        /// Size of the payload: only the command
        static const uint8_t SZ_PAYLOAD = SZ_CMD;

        /**
         * @brief Creates a READ command.
         *
         */
        Read();

        /**
         * @brief Copy constructor.
         *
         * @param[in] cmd command source
         */
        Read(const Read &cmd);

        /**
         * @brief Destroys command.
         *
         * @param[in] cmd command source
         */
        virtual ~Read();

        /**
         * @brief Creates the sequences of bytes for the READ command.
         *
         * This function creates the output command and saves it on a buffer.
         *
         * @param[out] buffer array where command is saved.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte written. 0 if there was an error.
         */
        virtual uint8_t serialize(uint8_t *buffer, size_t size);

      protected:
        /**
         * @brief Creates the type field of the READ command.
         *
         * This function inserts at the beginning of the buffer the command
         * type as an ASCII string.
         *
         * @param[out] buffer array where field is added.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte written. 0 if there was an error.
         */
        virtual uint8_t createFieldType(uint8_t *buffer, uint8_t size);

    };

    /**
     * @brief Class to describe a INFO command.
     *
     * The INFO command in Libelium SX1272 LoRa protocol contains
     * the current configuration of a LoRa module. This class implements the
     * Command and InputCommand interfaces.
     *
     * The INFO command has this structure:
     *
     *   - SOH character (Start of Header – ASCII 0×01)
     *   - command: INFO
     *   - Separator # (ASCII 0×23)
     *   - Setup data “;” separated. Each data will have the format “NAME:value”. The fields of this command are: FREC, ADDR, BW, CR, SF, RSSI and SNR. Last two parameters are optional.      *   - CR+LF characters (Carry return + line feed -ASCII 0x0D and 0x0A).
     *   - CR+LF characters (Carry return + line feed -ASCII 0x0D and 0x0A).
     *   - CRC16-Modbus code of the bytes between SOH and CR+LF (not including these)
     *   - EOT character (End of Transmission – ASCII 0×04)
     *
     * The frame would be:
     * [SOH]INFO#FREC:CH_13_868;ADDR:3;BW:BW_125;CR:CR_6;SF:SF_6;RSSI:32;SNR:21[CR+LF]FB05[EOT]
     *
     */
    class Info: public ConfigCommand, public InputCommand
    {
      public:

        /// Size of the command field: 4 bytes (ASCII string).
        static const uint8_t SZ_CMD = 4;

        /// Size of the payload: only the command
        static const uint8_t SZ_PAYLOAD = 0;

        /**
         * @brief Creates a INFO command.
         *
         */
        Info();

        /**
         * @brief Copy constructor.
         *
         * @param[in] cmd command source
         */
        Info(const Info &cmd);

        /**
         * @brief Destroys command.
         *
         * @param[in] cmd command source
         */
        virtual ~Info();

        /**
         * @brief Creates command from a byte array.
         *
         * This function creates an input command looking for a command in a array
         * of bytes.
         *
         * @param[out] buffer array of the received bytes.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte processed. 0 if there was an error.
         */
        virtual uint8_t createFromBuffer(uint8_t *buffer, size_t size);

      protected:
        /**
         * @brief Creates the type field of the INFO command.
         *
         * This function inserts at the beginning of the buffer the command
         * type as an ASCII string.
         *
         * @param[out] buffer array where field is added.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte written. 0 if there was an error.
         */
        virtual uint8_t createFieldType(uint8_t *buffer, uint8_t size);

    };

    /**
     * @brief Class to describe a SET command.
     *
     * The SET command in Libelium SX1272 LoRa protocol is used for writing
     * a new configuration in the LoRa module. This class implements the
     * Command and OutputCommand interfaces.
     *
     * The SET command has this structure:
     *
     *   - SOH character (Start of Header – ASCII 0×01)
     *   - command: SET
     *   - Separator # (ASCII 0×23)
     *   - Configuration data separated by ";". Each parameter has the format: NAME:value. Fields required are: FREC, ADDR, BW, CR and SF. Accepted values are:
     *       - FREC (frequency and channel) : CH_X_Y, where X is the channel and Y the frequency band. Bands allowed are 900 and 868 (MHz). Channels allowed are 10 to 17 for 868 MHz band and 00 to 12 for 900 MHz band.
     *       - ADDR (address): Number between 1 and 255.
     *       - BW (Bandwidth): BW_X where X can be 125, 250 or 500 KHz.
     *       - CR (coding rate): CR_X where X is a number between 5 and 8.
     *       - SF (Spreading factor): SF_X where X is a number between 6 and 12.
     *   - CR+LF characters (Carry return + line feed -ASCII 0x0D and 0x0A).
     *   - CRC16-Modbus code of the bytes between SOH and CR+LF (not including these)
     *   - EOT character (End of Transmission – ASCII 0×04)
     *
     * For example:
     * [SOH]SET#FREC:CH_13_868;ADDR:3;BW:BW_125;CR:CR_5;SF:SF_12[CR+LF]245A[EOT]
     *
     * The same command in hexadecimal format:
     * 01 53 45 54 23 46 52 45 43 3a 43 48 5f 31 33 5f 38 36 38 3b 41 44 44 52 3a 33 3b 42 57 3a 42 57 5f 31 32 35 3b 43 52 3a 43 52 5f 35 3b 53 46 3a 53 46 5f 31 32 0d 0a 32 34 35 41 04
     *
     */
    class Set: public ConfigCommand, public OutputCommand
    {
      public:

        /// Size of the command field: 4 bytes (ASCII string).
        static const uint8_t SZ_CMD = 3;

        /// Size of the payload: only the command
        static const uint8_t SZ_PAYLOAD = 0;

        /**
         * @brief Creates a SET command.
         *
         */
        Set();

        /**
         * @brief Copy constructor.
         *
         * @param[in] cmd command source
         */
        Set(const Set &msg);

        /**
         * @brief Destroys command.
         *
         * @param[in] cmd command source
         */
        virtual ~Set();

        /**
         * @brief Creates the sequences of bytes for the SET command.
         *
         * This function creates the output command and saves it on a buffer.
         *
         * @param[out] buffer array where command is saved.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte written. 0 if there was an error.
         */
        virtual uint8_t serialize(uint8_t *buffer, size_t size);

      protected:
        /**
         * @brief Creates the type field of the SET command.
         *
         * This function inserts at the beginning of the buffer the command
         * type as an ASCII string.
         *
         * @param[out] buffer array where field is added.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte written. 0 if there was an error.
         */
        virtual uint8_t createFieldType(uint8_t *buffer, uint8_t size);

        /**
         * @brief Creates the data field of the SET command.
         *
         * This function inserts at the beginning of the buffer the command
         * type as an ASCII string.
         *
         * @param[out] buffer array where field is added.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte written. 0 if there was an error.
         */
        virtual uint8_t createPayload(uint8_t *buffer, size_t size);

    };


    /**
     * @brief Class to describe a ERROR command.
     *
     * The ERROR command in Libelium SX1272 LoRa protocol is used to notifies
     * an error in the information received. This class implements the
     * Command and InputCommand interfaces.
     *
     * The ERROR command has this structure:
     *
     *   - SOH character (Start of Header – ASCII 0×01)
     *   - command: ERROR
     *   - Separator # (ASCII 0×23)
     *   - Error Message in ASCII. This will be an explicit message of what the error is.
     *   - CR+LF characters (Carry return + line feed -ASCII 0x0D and 0x0A).
     *   - CRC16-Modbus code of the bytes between SOH and CR+LF (not including these)
     *   - EOT character (End of Transmission – ASCII 0×04)
     *
     * The frame would be:
     * [SOH]ERROR#BAD CRC[CR+LF]6BED[EOT]
     *
     */
    class Error: public lora::Command, public InputCommand
    {
      public:

        /// Size of the command field: 4 bytes (ASCII string).
        static const uint8_t SZ_CMD = 5;

        /// Size of the payload: only the command
        static const uint8_t SZ_PAYLOAD = 0;

        /**
         * @brief Creates a ERROR command.
         *
         */
        Error();

        /**
         * @brief Copy constructor.
         *
         * @param[in] cmd command source
         */
        Error(const Error &cmd);

        /**
         * @brief Destroys command.
         *
         * @param[in] cmd command source
         */
        virtual ~Error();

        /**
         * @brief Creates command from a byte array.
         *
         * This function creates an input command looking for a command in a array
         * of bytes.
         *
         * @param[out] buffer array of the received bytes.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte processed. 0 if there was an error.
         */
        virtual uint8_t createFromBuffer(uint8_t *buffer, size_t size);

        /**
         * @brief Gets the error description.
         *
         * This function returns the error description.
         *
         * @returns the error description.
         */
        const std::string& error()
        {
          return m_error;
        }
        ;

      protected:
        /**
         * @brief Creates the type field of the ERROR command.
         *
         * This function inserts at the beginning of the buffer the command
         * type as an ASCII string.
         *
         * @param[out] buffer array where field is added.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte written. 0 if there was an error.
         */
        virtual uint8_t createFieldType(uint8_t *buffer, uint8_t size);

        //! Error message
        std::string m_error;

    };

    /**
     * @brief Class to describe a DATA command for ASCII messages.
     *
     * The DATA command in Libelium SX1272 LoRa protocol is used for sending
     * data. The structure of this command is:
     *
     * The DATA command has this structure:
     *
     *   - SOH character (Start of Header – ASCII 0×01)
     *   - command: DATA
     *   - Separator # (ASCII 0×23)
     *   - Destination address: Number between 1 and 255. 0 for broadcast message.
     *   - Separator # (ASCII 0×23)
     *   - Message type. Allowed types are ASCII (text) and BIN (binary). If binary message, you have to append the length of the message in bytes, separated by “;”. For example: BIN;60.
     *   - Separator # (ASCII 0×23)
     *   - Message to send. If you want to send non-printable ASCII chars you need to escape them before with the character ESCAPE (ASCII 0x1B) and the character you want to send. You have to escape too the character # (ASCII 0×23) in order to put it in a message.
     *   - CR+LF characters (Carry return + line feed -ASCII 0x0D and 0x0A).
     *   - CRC16-Modbus code of the bytes between SOH and CR+LF (not including these)
     *   - EOT character (End of Transmission – ASCII 0×04)
     *
     * For example:
     * [SOH]DATA#2#ASCII#THIS IS THE MESSAGE[CR+LF]9DD5[EOT]
     *
     * The same command in hexadecimal format:
     * 01 44 41 54 41 23 32 23 41 53 43 49 49 23 54 48 49 53 20 49 53 20 54 48 45 20 4d 45 53 53 41 47 45 0d 0a 39 44 44 35 04
     *
     */
    class Data: public lora::Command, public OutputCommand
    {
      public:

        /// Size of the command field: 4 bytes (ASCII string).
        static const uint8_t SZ_CMD = 4;

        /// Size of the payload: only the command
        static const uint8_t SZ_PAYLOAD = 0;

        /**
         * @brief Creates a DATA command.
         *
         */
        Data();

        /**
         * @brief Copy constructor.
         *
         * @param[in] cmd command source
         */
        Data(const Data &msg);

        /**
         * @brief Destroys command.
         *
         * @param[in] cmd command source
         */
        virtual ~Data();

        /**
         * @brief Creates the sequences of bytes for the DATA command.
         *
         * This function creates the output command and saves it on a buffer.
         *
         * @param[out] buffer array where command is saved.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte written. 0 if there was an error.
         */
        virtual uint8_t serialize(uint8_t *buffer, size_t size);

        /**
         * @brief Sets the destination node address.
         *
         * This function sets the destination node address. It is a number between 1 and
         * 255 (0 is for broadcast message).
         *
         * @param[in] dst address of the destination node. 0 for broadcast message.
         *
         * @returns false if node address value is valid, true otherwise.
         */
        void setDest(uint8_t dst)
        {
          m_dest = dst;
        }
        ;

        /**
         * @brief Gets the destination node address.
         *
         * This function returns the destination node address.
         *
         * @returns destination node address.
         */
        uint8_t dest()
        {
          return m_dest;
        }

        /**
         * @brief Sets the message to an ASCII string.
         *
         * This function sets the message to an ASCII string.
         *
         * @param[in] data ASCII string to send.
         *
         */
        void setData(std::string& data)
        {
          m_data = data;
        }
        ;

        /**
         * @brief Gets the message field.
         *
         * This function returns the message field.
         *
         * @returns the message to send.
         */
        const std::string& data()
        {
          return m_data;
        }
        ;

      protected:
        /**
         * @brief Creates the type field of the DATA command.
         *
         * This function inserts at the beginning of the buffer the command
         * type as an ASCII string.
         *
         * @param[out] buffer array where field is added.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte written. 0 if there was an error.
         */
        virtual uint8_t createFieldType(uint8_t *buffer, uint8_t size);

        /**
         * @brief Creates the data field of the DATA command.
         *
         * This function inserts at the beginning of the buffer the command
         * type as an ASCII string.
         *
         * @param[out] buffer array where field is added.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte written. 0 if there was an error.
         */
        virtual uint8_t createPayload(uint8_t *buffer, size_t size);

        //! Destination address
        uint8_t m_dest;

        //! Message to send
        std::string m_data;

    };

    /**
     * @brief Class to describe a ACK command.
     *
     * The ACK command in Libelium SX1272 LoRa protocol is used to notifies
     * that a message has been successfully sent. This class implements the
     * Command and InputCommand interfaces.
     *
     * The ACK command has this structure:
     *
     *   - SOH character (Start of Header – ASCII 0×01)
     *   - command: ACK
     *   - CR+LF characters (Carry return + line feed -ASCII 0x0D and 0x0A).
     *   - CRC16-Modbus code of the bytes between SOH and CR+LF (not including these)
     *   - EOT character (End of Transmission – ASCII 0×04)
     *
     * The frame would be:
     * [SOH]ACK[CR+LF]D350[EOT]
     *
     */
    class Ack: public lora::Command, public InputCommand
    {
      public:

        /// Size of the command field: 4 bytes (ASCII string).
        static const uint8_t SZ_CMD = 3;

        /// Size of the payload: only the command
        static const uint8_t SZ_PAYLOAD = 0;

        /**
         * @brief Creates a ACK command.
         *
         */
        Ack();

        /**
         * @brief Copy constructor.
         *
         * @param[in] cmd command source
         */
        Ack(const Ack &cmd);

        /**
         * @brief Destroys command.
         *
         * @param[in] cmd command source
         */
        virtual ~Ack();

        /**
         * @brief Creates command from a byte array.
         *
         * This function creates an input command looking for a command in a array
         * of bytes.
         *
         * @param[out] buffer array of the received bytes.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte processed. 0 if there was an error.
         */
        virtual uint8_t createFromBuffer(uint8_t *buffer, size_t size);

      protected:
        /**
         * @brief Creates the type field of the ACK command.
         *
         * This function inserts at the beginning of the buffer the command
         * type as an ASCII string.
         *
         * @param[out] buffer array where field is added.
         * @param[in] size size of the buffer (number of bytes).
         *
         * @returns number of byte written. 0 if there was an error.
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
