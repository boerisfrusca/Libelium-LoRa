//============================================================================
// Name        : interfaces.h
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Interface for Lo-Ra commands
//============================================================================
#ifndef INTERFACES_H_
#define INTERFACES_H_

#include <stdint.h>
#include <stdio.h>
#include <string>

namespace lora
{
  /**
   * Interface for a generic LoRa Command.
   */
  class Command
  {
    public:
      /// Maximum Payload length
      static const uint8_t PAYLOAD_MAX_LENGTH = 255;

      /// Header size in bytes (byte SOH)
      static const uint8_t SZ_START = 1;

      /// Size of the command field in bytes.
      static const uint8_t SZ_CMD = 0;

      /// Size of the payload in bytes.
      static const uint8_t SZ_PAYLOAD = 0;

      /// Size of the field separator in bytes.
      static const uint8_t SZ_FS = 2;

      /// Size of the separator between payload and CRC. It is 2 bytes (CR + LF).
      static const uint8_t SZ_SEPARATOR = 2;

      /// CRC size as binary value
      static const uint8_t SZ_CRC_BINARY = 2;

      /// CRC size in bytes. 2 binary bytes becomes 4 ASCII byte (hexadecimal number)
      static const uint8_t SZ_CRC = SZ_CRC_BINARY * 2;

      /// End sequence size in bytes (byte EOT)
      static const uint8_t SZ_END = 1;

      /**
       * Special characters
       */
      enum S_CHAR
      {
        /// Start of Header
        SOH = 0x01,

        /// Carry return
        CR = 0x0D,

        /// Line feed
        LF = 0x0A,

        /// End of Transmission
        EOT = 0x04,

        /// Field Separator
        FS = 0x23,
      };

      /**
       * Waspmote LoRa message type
       */
      enum CMD_TYPE
      {
        /// Unknown
        UNKNOWN = 0x00,

        /// READ - Gets the current setup of the module
        READ = 0x01,

        /// SET - Sets a new configuration of the model
        SET = 0x02,

        /// DATA - Sends data from the module to a Waspmote
        DATA = 0x03,

        /// ERROR - Notifies an error in the information received
        ERROR = 0x04,

        /// INFO - With this command, the module gives its setup
        INFO = 0x05,

        /// ACK -  With this command the module notifies that a message has been successfully sent
        ACK = 0x06
      };

      /**
       * Error code
       */
      enum _ERROR_CODE
      {
        NO_ERROR = 0x00,
        NULL_BUFFER_IN = 0x01,
        NULL_BUFFER_OUT = 0x02,
        INVALID_CMD = 0x03,
        INVALID_PAYLOAD_1 = 0x04,
        INVALID_PAYLOAD_2 = 0x05,
        INVALID_CRC = 0x07,
        INVALID_EOT = 0x08,
        INVALID_MSG = 0x09,
      };

      /**
       * Calculate modbud CRC16.
       *
       * \param[in] buf data buffer
       * \param[in] len buffer size (number of bytes)
       *
       * \return CRC
       */
      static uint16_t CRC16(uint8_t *buf, size_t len);

      /**
       * Creates an empty LoRa command for WaspMote.
       *
       */
      Command() :
          m_type(UNKNOWN), m_crc(0), m_size(0)
      {
      }
      ;

      /**
       * Copy constructor
       *
       * \param cmd command source
       */
      Command(const Command &cmd)
      {
        m_type = cmd.m_type;
        m_crc = cmd.m_crc;
        m_size = cmd.m_size;
      }
      ;

      /**
       * Destroy a serial device object.
       *
       */
      virtual ~Command()
      {
      }
      ;

      /**
       * Gets the command type
       *
       * \return type of the command, according to lora::CMD_TYPE
       */
      inline uint8_t type()
      {
        return m_type;
      }

      /**
       * Gets the modbus CRC16
       *
       * \return CRC
       */
      inline uint16_t crc()
      {
        return m_crc;
      }

      /**
       * Returns the message size (number of byte)
       *
       * \return the size of the message in number of byte.
       */
      virtual uint8_t size()
      {
        return m_size;
      }
      ;

      /**
       * Processes a buffer of size "sz" and extract the command type, data payload and CRC value.
       *
       * \param[in] buffer buffer to process
       * \param[in] sz size of the buffer to process
       * \param[out] type command type
       * \param[out] payload buffer where data-paylod is saved
       * \param[out] p_size size of the payload buffer
       * \param[out] crc Modbus CRC 16 contained in the received message
       *
       * \return lora::Command::NO:ERROR if a command is extracted from the buffer, a lora::Command::_ERROR_CODE code otherwise
       */
      static uint8_t process(const uint8_t *buffer, const size_t sz, uint8_t &type,
          uint8_t *payload, size_t &p_size, uint16_t &crc);

    protected:
      /**
       * Processes a textual string (field) with the following format: "<tag><fs><value>". This functions
       * returns the value as a std::sting.
       *
       * \param[in] field string to process
       * \param[in] tag label of the value to extract
       * \param[in] fs field separator
       * \param[out] val value to return
       *
       * \return true  if tag is found, false in case of errors.
       */
      bool processField(const std::string &field, const std::string &tag, const std::string &fs,
          std::string &value);

      /**
       * Processes a textual string (field) with the following format: "<tag><fs><value>". This functions
       * returns the value as a long number.
       *
       * \param[in] field string to process
       * \param[in] tag label of the value to extract
       * \param[in] fs field separator
       * \param[out] val value to return
       *
       * \return true  if tag is found, false in case of errors.
       */
      bool processField(const std::string &field, const std::string &tag, const std::string &fs,
          long &value);

      /**
       * Creates message header.
       *
       * \param buffer array if byte where store he message
       * \param size size of the buffer in number of bytes
       *
       * \return number of byte written into the buffer. 0 if there was an error.
       */
      virtual uint8_t createFieldStart(uint8_t *buffer, uint8_t size);

      /**
       * Creates the payload of the message.
       *
       * \param buffer array where message payload is stored
       * \param size size of the buffer in number of bytes
       *
       * \return number of byte processed from the buffer. 0 if there was an error.
       */
      virtual uint8_t createPayload(uint8_t *buffer, size_t size);

      /**
       * Creates field with the separator (before CRC).
       *
       * \param buffer array if byte where store he message
       * \param size size of the buffer in number of bytes
       *
       * \return number of byte written into the buffer. 0 if there was an error.
       */
      virtual uint8_t createFieldSeparator(uint8_t *buffer, uint8_t size);

      /**
       * Creates field with the CRC.
       *
       * \param buffer array if byte where store he message
       * \param size size of the buffer in number of bytes
       *
       * \return number of byte written into the buffer. 0 if there was an error.
       */
      virtual uint8_t createFieldCRC(uint8_t *buffer, uint8_t size);

      /**
       * Creates message header.
       *
       * \param buffer array if byte where store he message
       * \param size size of the buffer in number of bytes
       *
       * \return number of byte written into the buffer. 0 if there was an error.
       */
      virtual uint8_t createFieldEnd(uint8_t *buffer, uint8_t size);

      /**
       * Creates field type
       *
       * \param buffer array if byte where store he message
       * \param size size of the buffer in number of bytes
       *
       * \return number of byte written into the buffer. 0 if there was an error.
       */
      virtual uint8_t createFieldType(uint8_t *buffer, uint8_t size) = 0;

      //! Message type
      uint8_t m_type;

      //! ModBus CRC16
      uint16_t m_crc;

      //! Message size
      uint8_t m_size;
  };

  class OutputCommand
  {
    public:

      /**
       * Creates an empty LoRa message for WaspMote.
       *
       */
      OutputCommand()
      {
      }
      ;

      /**
       * Copy constructor
       *
       * \param msg message source
       */
      OutputCommand(const OutputCommand &cmd)
      {
      }
      ;

      /**
       * Destroy a serial device object.
       *
       */
      virtual ~OutputCommand()
      {
      }
      ;

      /**
       * Serialize a message storing it into a buffer
       *
       * \param buffer array if byte where store he message
       * \param size size of the buffer in number of bytes
       *
       * \return number of byte written into the buffer. 0 if there was an error.
       */
      virtual uint8_t serialize(uint8_t *buffer, size_t size) = 0;
  };

  class InputCommand
  {
    public:

      /**
       * Creates an empty LoRa message for WaspMote.
       *
       */
      InputCommand()
      {
      }
      ;

      /**
       * Copy constructor
       *
       * \param msg message source
       */
      InputCommand(const InputCommand &cmd)
      {
      }
      ;

      /**
       * Destroy a serial device object.
       *
       */
      virtual ~InputCommand()
      {
      }
      ;

      /**
       * Creates message from buffer.
       *
       * \param buffer array where message payload is stored
       * \param size size of the buffer in number of bytes
       *
       * \return number of byte processed from the buffer. 0 if there was an error.
       */
      virtual uint8_t createFromBuffer(uint8_t *buffer, size_t size) = 0;
  };

  class ConfigCommand : public Command
  {
    public:

      /**
       * LoRa Frequency. Frequency bands allowed are 868 and 900 MHz.
       */
      enum _FREQUENCY
      {
        /// Undefined Channel
        F_UNKN = 0xFF,

        /// 868 MHz
        F_868 = 0x00,

        /// 900 MHz
        F_900 = 0x01,
      };

      /**
       * LoRa channel. Channels allowed are 10 to 17 for 868 MHz band and 00 to 12 for 900 MHz band.
       */
      enum _FREQUENCY_CHANNEL
      {
        /// Undefined Channel
        CH_UNKN = 0xFF,

        /// Channel 0
        CH_00 = 0x00,

        /// Channel 1
        CH_01 = 0x01,

        /// Channel 2
        CH_02 = 0x02,

        /// Channel 3
        CH_03 = 0x03,

        /// Channel 4
        CH_04 = 0x04,

        /// Channel 5
        CH_05 = 0x05,

        /// Channel 6
        CH_06 = 0x06,

        /// Channel 7
        CH_07 = 0x07,

        /// Channel 8
        CH_08 = 0x08,

        /// Channel 9
        CH_09 = 0x09,

        /// Channel 10
        CH_10 = 0x0A,

        /// Channel 11
        CH_11 = 0x0B,

        /// Channel 12
        CH_12 = 0x0C,

        /// Channel 13
        CH_13 = 0x0D,

        /// Channel 14
        CH_14 = 0x0E,

        /// Channel 15
        CH_15 = 0x0F,

        /// Channel 16
        CH_16 = 0x10,

        /// Channel 17
        CH_17 = 0x11,
      };

      /**
       * LoRa channel bandwidth. Bandwidth allowed are 125, 250 and 500 KHz.
       */
      enum _BANDWIDTH
      {
        /// Undefined bandwidth
        BW_UNKN = 0xFF,

        /// Bandwidth 125 KHz
        BW_125 = 0x00,

        /// Bandwidth 250 KHz
        BW_250 = 0x01,

        /// Bandwidth 500 KHz
        BW_500 = 0x02,
      };

      /**
       * LoRa coding rate. It must be a number between 5 and 8
       */
      enum _CODING_RATE
      {
        /// Undefined coding rate
        CR_UNKN = 0xFF,

        /// Coding rate 5
        CR_5 = 0x00,

        /// Coding rate 5
        CR_6 = 0x01,

        /// Coding rate 5
        CR_7 = 0x02,

        /// Coding rate 5
        CR_8 = 0x03,
      };

      /**
       * LoRa spreading factor. It must be a value number 6 and 12
       */
      enum _SPREADING_FACTOR
      {
        /// Undefined spreading factor
        SF_UNKN = 0xFF,

        /// Spreading factor 6
        SF_6 = 0x00,

        /// Spreading factor 7
        SF_7 = 0x01,

        /// Spreading factor 8
        SF_8 = 0x02,

        /// Spreading factor 9
        SF_9 = 0x03,

        /// Spreading factor 0
        SF_10 = 0x04,

        /// Spreading factor 11
        SF_11 = 0x05,

        /// Spreading factor 12
        SF_12 = 0x06,
      };

      /**
       * Creates a CFG LoRa message for WaspMote.
       *
       */
      ConfigCommand();

      /**
       * Copy constructor
       *
       * \param msg message source
       */
      ConfigCommand(const ConfigCommand &cmd);

      /**
       * Destroy a serial device object.
       *
       */
      virtual ~ConfigCommand();

      /**
       * Sets frequency band to a code value according to MessageCFG::_FREQUENCY enum.
       * Frequency bands allowed are 868 and 900 MHz, for other values frequency will be considered
       * as unknown and not valid.
       *
       * \param[in] freq frequency code.
       *
       * \return false if frequency value is not allowed, true otherwise.
       */
      bool setFrequency(uint8_t freq);

      /**
       * Gets frequency band. If code parameter is true, function returns the frequency code according to
       * MessageCFG::_FREQUENCY enum, otherwise it returns the frequency value in MHz.
       *
       * \param code flag to select return value as frequency code or MHz value.
       *
       * \return the frequency code value .
       */
      uint16_t frequency(bool code = true);

      /**
       * Gets frequency band ad a printable string.
       *
       * \return the frequency band in MHz.
       */
      std::string frequencyAsString();

      /**
       * Sets LoRa channel to a code value according to MessageCFG::_FREQUENCY_CH enum.
       * Channels allowed are 10 to 17 for 868 MHz band and 00 to 12 for 900 MHz band.
       * Other values frequency will be considered as unknown and not valid.
       *
       * \param[in] ch channel code.
       *
       * \return false if channel value is not allowed, true otherwise.
       */
      bool setChannel(uint8_t ch);

      /**
       * Gets the channel value. If code parameter is true, function returns the channel code according to
       * MessageCFG::_CHANNEL enum, otherwise it returns the channel value.
       *
       * \param code flag to select return value as channel code or value.
       *
       * \return the channel.
       */
      uint8_t channel(bool code = true);

      /**
       * Gets channel ad a printable string.
       *
       * \return the channel .
       */
      std::string channelAsString();

      /**
       * Sets the node address. It is a number between 1 and 255.
       *
       * \param[in] addr channel code.
       *
       * \return false if address value is not valid, true otherwise.
       */
      inline bool setAddress(uint8_t addr)
      {
        m_addr = addr;
        return (m_addr > 0);
      }

      /**
       * Gets the node address.
       *
       * \return node address.
       */
      inline uint8_t address()
      {
        return m_addr;
      }

      /**
       * Sets LoRa channel bandwidth to a code value according to MessageCFG::_BANDWIDTH enum.
       * Bandwidth allowed are 125, 250 and 500 KHz.
       * Other values frequency will be considered as unknown and not valid.
       *
       * \param[in] bw bandwidth code.
       *
       * \return false if bandwidth value is not allowed, true otherwise.
       */
      bool setBandwidth(uint8_t bw);

      /**
       * Gets the bandwidth. If code parameter is true, function returns the bandwidth code according to
       * MessageCFG::_BANDWIDTH enum, otherwise it returns the bandwidth value.
       *
       * \param code flag to select return value as bandwidth code or value in KHz.
       *
       * \return the channel.
       */
      uint16_t bandwidth(bool code = true);

      /**
       * Gets bandwidth ad a printable string.
       *
       * \return the bandwidth.
       */
      std::string bandwidthAsString();


      /**
       * Sets LoRa coding-rate. It must be a number between 5 and 8.
       *
       * \param[in] cr coding-rate.
       *
       * \return false if coding-rate value is not allowed, true otherwise.
       */
      bool setCodingRate(uint8_t cr);

      /**
       * Gets the coding-rate.
       *
       * \return the coding-rate value according to MessageCFG::_CODING_RATE enum.
       */

      /**
       * Gets the coding-rate value. If code parameter is true, function returns the coding-rate code according to
       * MessageCFG::_CODING_RATE enum, otherwise it returns the coding-rate value.
       *
       * \param code flag to select return value as coding-rate code or value.
       *
       * \return the coding-rate.
       */
      uint8_t codingRate(bool code = true);

      /**
       * Gets coding-rate ad a printable string.
       *
       * \return the coding-rate .
       */
      std::string codingRateAsString();


      bool setSpreadingFactor(uint8_t sf);

      /**
       * Gets the spreading factor value. If code parameter is true, function returns the spreading factor code according to
       * MessageCFG::_SPREADING_FACTOR enum, otherwise it returns the spreading factor value.
       *
       * \param code flag to select return value as spreading factor code or value.
       *
       * \return the spreading factor.
       */
      uint8_t spreadingFactor(bool code = true);

      /**
       * Gets spreading factor ad a printable string.
       *
       * \return the spreading factor .
       */
      std::string spreadingFactorAsString();

      inline int rssi()
      {
        return m_rssi;
      }

      inline int snr()
      {
        return m_snr;
      }

      inline int rssi_pck()
      {
        return m_rssi_pck;
      }

    protected:
      //! LoRa frequency
      uint8_t m_freq;

      //! LoRa channel
      uint8_t m_ch;

      //! Address
      uint8_t m_addr;

      //! Bandwidth
      uint8_t m_bw;

      //! Coding rate
      uint8_t m_cr;

      //! Spreading factor
      uint8_t m_sf;

      //! RSSI
      int m_rssi;

      //! RSSI packet
      int m_rssi_pck;

      //! SNR
      int m_snr;
  };

} /* namespace lora */

#endif /* INTERFACES_H_ */
