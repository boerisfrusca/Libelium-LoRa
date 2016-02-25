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
   * @brief Interface for a generic LoRa Command.
   *
   * The Libelium SX1272 LoRa protocol uses some special characters that
   * ensure the orders are properly read and the a checksum to avoid possible
   * transmission errors.
   * Special characters are:
   *
   *    - SOH : start of header (ASCII 0x01).
   *    - CR + LF: carry return and line feed (ASCII 0x0D + 0x0A).
   *    - EOT : end of transmission (ASCII 0x04).
   *
   * Error check is based on modbus CRC16.
   *
   * The Libelium SX1272 LoRa module uses the following command:
   *
   *    - \bREAD  : gets the current setup of the module.
   *    - \bSET   : sets a new configuration in the module.
   *    - \bDATA  : sends data from the module to a Waspmote.
   *    - \bERROR : notifies an error in the information received.
   *    - \bINFO  : with this command, the modules gives its setup.
   *    - \bACK   : with this command, the modules notifies that a message has been successfully received.
   *
   * Command are a sequence of bytes with the following structure:
   *
   * | SOH | Command_Type | <Data_Separator> | <Data> | CR | LF | CRC16 | EOT |
   *
   * where:
   *    - Command_Type : command name in ASCII
   *    - Data_Separator : optional field. It is a character ('#', ASCII 0x23) and separates the Command_Type field from the Data.
   *    - Data : optional field. It is a ASCII characters sequence.
   *    - CRC16 : code for the bytes between SOH and CR + LF (not including these). It is 2 hexadecimal bytes and they are codify as ASII string (e.g. 0x2A31 becomes 0x32, 0x41, 0x33, 0x31).
   *
   */
  class Command
  {
    public:
      /// Maximum length of the payload field (<Data_Separator> + <Data>)
      static const uint8_t PAYLOAD_MAX_LENGTH = 255;

      /// Header size in bytes (byte SOH)
      static const uint8_t SZ_START = 1;

      /// Size of the command field in bytes. Values 0 means unknown.
      static const uint8_t SZ_CMD = 0;

      /// Size of the payload in bytes. Values 0 means unknown.
      static const uint8_t SZ_PAYLOAD = 0;

      /// Size of the field separator in bytes.
      static const uint8_t SZ_FS = 2;

      /// Size of the separator before the CRC. It is 2 bytes (CR + LF).
      static const uint8_t SZ_SEPARATOR = 2;

      /// CRC size as binary value
      static const uint8_t SZ_CRC_BINARY = 2;

      /// CRC size as ASCII string. 2 binary bytes becomes 4 ASCII byte (hexadecimal number).
      static const uint8_t SZ_CRC = SZ_CRC_BINARY * 2;

      /// End sequence size in bytes (byte EOT)
      static const uint8_t SZ_END = 1;

      /**
       * @brief Special characters.
       *
       *  The Libelium SX1272 LoRa protocol uses some special characters that
       *  ensure the orders are properly read and the a checksum to avoid possible
       *  transmission errors.
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
       * @brief Special characters.
       *
       *  Commands implemented by the Libelium SX1272 LoRa protocol.
       */
      enum CMD_TYPE
      {
        /// Unknown or not valid command.
        UNKNOWN = 0x00,

        /// READ : gets the current setup of the module.
        READ = 0x01,

        /// SET : sets a new configuration of the model.
        SET = 0x02,

        /// DATA : sends data from the module to a Waspmote.
        DATA = 0x03,

        /// ERROR : notifies an error in the information received.
        ERROR = 0x04,

        /// INFO - With this command, the module gives its setup
        INFO = 0x05,

        /// ACK -  With this command the module notifies that a message has been successfully sent
        ACK = 0x06
      };

      /**
       * @brief Error codes returned by functions.
       */
      enum _ERROR_CODE
      {
        /// No error.
        NO_ERROR = 0x00,

        /// Input buffer undefined.
        NULL_BUFFER_IN = 0x01,

        /// Output buffer undefined.
        NULL_BUFFER_OUT = 0x02,

        /// Invalid command type.
        INVALID_CMD = 0x03,

        /// Error while searching the payload.
        INVALID_PAYLOAD_1 = 0x04,

        /// Error while searching the payload, not found CR + LF.
        INVALID_PAYLOAD_2 = 0x05,

        /// Invalid CRC field.
        INVALID_CRC = 0x07,

        /// EOT not found.
        INVALID_EOT = 0x08,

        /// Command not found.
        CMD_NOT_FOUND = 0x09,
      };

      /**
       * @brief Calculates modbud CRC16.
       *
       * This function calculates the modbus checksum on a byte array and returns it as number on 2 bytes.
       *
       * @param[in] buf data buffer.
       * @param[in] len buffer size (number of bytes)
       *
       * @returns CRC16 value.
       */
      static uint16_t CRC16(uint8_t *buf, size_t len);

      /**
        * @brief Creates an empty LoRa command for WaspMote.
        *
        */
      Command() :
          m_type(UNKNOWN), m_crc(0), m_size(0)
      {
      }
      ;

      /**
        * @brief Copy constructor.
        *
        * @param[in] cmd command source object.
        *
        */
      Command(const Command &cmd)
      {
        m_type = cmd.m_type;
        m_crc = cmd.m_crc;
        m_size = cmd.m_size;
      }
      ;

      /**
        * @brief Destroys a command.
        *
        */
     virtual ~Command()
      {
      }
      ;

     /**
      * @brief Gets the command type.
      *
      * This function returns the command type as a code according to
      * lora::Command::CMD_TYPE enum.
      *
      * @returns command type.
      */
      inline uint8_t type()
      {
        return m_type;
      }

      /**
       * @brief Gets the checksum.
       *
       * This function returns the checksum as a number on 2 bytes.
       *
       * @returns CRC.
       */
      inline uint16_t crc()
      {
        return m_crc;
      }

      /**
       * @brief Gets the size of the message.
       *
       * This function returns the length of the message. If it is 0, means size is unknown.
       *
       * @returns message length.
       */
      virtual uint8_t size()
      {
        return m_size;
      }
      ;

      /**
       * @brief Processes a buffer of size "sz" bytes.
       *
       * This function processes a buffer of size "sz" bytes and extract command
       * type, payload (<Data_Separator> + <Data>), payload length and CRC value.
       *
       * @param[in] buffer buffer to process
       * @param[in] sz size of the buffer to process
       * @param[out] type command type
       * @param[out] payload buffer where data-paylod is saved
       * @param[out] p_size size of the payload buffer
       * @param[out] crc Modbus CRC 16 contained in the received message
       *
       * @returns lora::Command::NO:ERROR if a command is extracted from the buffer, a lora::Command::_ERROR_CODE code otherwise.
       */
      static uint8_t process(const uint8_t *buffer, const size_t sz, uint8_t &type,
          uint8_t *payload, size_t &p_size, uint16_t &crc);

    protected:
      /**
       * @brief Processes a string (field) with the following format: "<tag><fs><value>".
       *
       * This function processes a string  with the following format: "
       * <tag><fs><value>" and it returns the value as a string if tag is found.
       *
       * @param[in] field string to process.
       * @param[in] tag label of the value to extract.
       * @param[in] fs field separator.
       * @param[out] val value to return.
       *
       * @returns true  if tag is found, false in case of errors.
       */
      bool processField(const std::string &field, const std::string &tag, const std::string &fs,
          std::string &value);

      /**
       * @brief Processes a string (field) with the following format: "<tag><fs><value>".
       *
       * This function processes a string  with the following format: "
       * <tag><fs><value>" and it returns the value as a long integer if tag is found.
       *
       * @param[in] field string to process.
       * @param[in] tag label of the value to extract.
       * @param[in] fs field separator.
       * @param[out] val value to return.
       *
       * @returns true  if tag is found, false in case of errors.
       */
      bool processField(const std::string &field, const std::string &tag, const std::string &fs,
          long &value);

      /**
       * @brief Creates command header.
       *
       * This function inserts in the first byte of a buffer the command header.
       *
       * @param[out] buffer array where field is added.
       * @param[in] size size of the buffer (number of bytes).
       *
       * @returns number of byte written. 0 if there was an error.
       */
      virtual uint8_t createFieldStart(uint8_t *buffer, uint8_t size);

      /**
       * @brief Creates payload of the command (<Data_Separator> + <Data>).
       *
       * This function inserts at the beginning of the buffer payload of the
       * command (<Data_Separator> + <Data>).
       *
       * @param[out] buffer array where field is added.
       * @param[in] size size of the buffer (number of bytes).
       *
       * @returns number of byte written. 0 if there was an error.
       */
      virtual uint8_t createPayload(uint8_t *buffer, size_t size);

      /**
       * @brief Creates field separator before the CRC.
       *
       * This function inserts at the beginning of the buffer the separator
       * between data and CRC ( CR + LF characters).
       *
       * @param[out] buffer array where field is added.
       * @param[in] size size of the buffer (number of bytes).
       *
       * @returns number of byte written. 0 if there was an error.
       */
      virtual uint8_t createFieldSeparator(uint8_t *buffer, uint8_t size);

      /**
       * @brief Creates the CRC field of the command.
       *
       * This function inserts at the beginning of the buffer the CRC
       * as a sequence of ASCII bytes.
       *
       * @param[out] buffer array where field is added.
       * @param[in] size size of the buffer (number of bytes).
       *
       * @returns number of byte written. 0 if there was an error.
       */
      virtual uint8_t createFieldCRC(uint8_t *buffer, uint8_t size);

      /**
       * @brief Creates the stop sequence of the command.
       *
       * This function inserts at the beginning of the buffer the EOT byte.
       *
       * @param[out] buffer array where field is added.
       * @param[in] size size of the buffer (number of bytes).
       *
       * @returns number of byte written. 0 if there was an error.
       */
      virtual uint8_t createFieldEnd(uint8_t *buffer, uint8_t size);

      /**
       * @brief Creates the type field of the command.
       *
       * This function inserts at the beginning of the buffer the command
       * type as an ASCII string.
       *
       * @param[out] buffer array where field is added.
       * @param[in] size size of the buffer (number of bytes).
       *
       * @returns number of byte written. 0 if there was an error.
       */
      virtual uint8_t createFieldType(uint8_t *buffer, uint8_t size) = 0;

      //! Code of the message type
      uint8_t m_type;

      //! ModBus CRC16
      uint16_t m_crc;

      //! Message size
      uint8_t m_size;
  };

  /**
   * @brief Interface for a generic command to send to the Libelium SX1272
   * LoRa module.
   *
   * An output command is a sequence of bytes corresponding to a LoRa command.
   * This byte stream is then written on the serial device so this interface
   * exports a function (serialize) for transforming the command description
   * in a byte sequence and storing it in a array (data buffer).
   */
  class OutputCommand
  {
    public:

      /**
       * @brief Creates an empty output command.
       *
       */
      OutputCommand()
      {
      }
      ;

      /**
       * @brief Copy constructor.
       *
       * @param[in] cmd otput command object.
       *
       */
      OutputCommand(const OutputCommand &cmd)
      {
      }
      ;

      /**
       * @brief Destroys an output command.
       *
       */
      virtual ~OutputCommand()
      {
      }
      ;

      /**
       * @brief Creates the sequences of bytes for the output command.
       *
       * This function creates the output command and saves it on a buffer.
       *
       * @param[out] buffer array where command is saved.
       * @param[in] size size of the buffer (number of bytes).
       *
       * @returns number of byte written. 0 if there was an error.
       */
      virtual uint8_t serialize(uint8_t *buffer, size_t size) = 0;
  };

  /**
   * @brief Interface for a generic command received from the Libelium SX1272
   * LoRa module.
   *
   * An input command can be created from a byte stream received from the
   * Libelium SX1272 LoRa module. This interface exports a function for
   * looking for a command in a data buffer (where received byte are saved)
   * and initializes all command attributes.
   */
  class InputCommand
  {
    public:

      /**
       * @brief Creates an empty input command.
       *
       */
      InputCommand()
      {
      }
      ;

      /**
       * @brief Copy constructor.
       *
       * @param[in] cmd input command object.
       *
       */
      InputCommand(const InputCommand &cmd)
      {
      }
      ;

      /**
       * @brief Destroys an input command.
       *
       */
      virtual ~InputCommand()
      {
      }
      ;

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
      virtual uint8_t createFromBuffer(uint8_t *buffer, size_t size) = 0;
  };

  /**
   * @brief Interface for a generic configuration LoRa Command.
   *
   * The Libelium SX1272 LoRa module allows to set up the following transmission parameter:
   *
   *    - ADDR : Node address.
   *    - FREQ : Frequency band.
   *    - CH   : Channel.
   *    - BW   : Bandwidth.
   *    - CR   : Coding rate.
   *    - SF   : Spreading factor.
   *
   *  It can returns some additional information as RSSI, packet RSSI and SNR.
   *
   *  Address must be a number between 1 and 255 (0 is for broadcast message).
   *  Frequency bands allowed are 868 MHz (Europe) and 900 MHz (US).
   *  Channels allowed are 10 to 17 for 868 MHz band and 0 to 12 for 900 MHZ band.
   *  Bandwidth can be 125, 250 or 500 KHz.
   *  Coding rate is a number between 5 and 8.
   *  Spreading factor is a number between 6 and 12.
   *
   */
  class ConfigCommand : public Command
  {
    public:

      /**
       * @brief Codes for frequency band supported by the LoRa module.
       *
       *  Frequency bands allowed are 868 MHz (Europe) and 900 MHz (US).
       */
      enum _FREQUENCY
      {
        /// Undefined or not valid frequency band.
        F_UNKN = 0xFF,

        /// 868 MHz (Europe)
        F_868 = 0x00,

        /// 900 MHz (US)
        F_900 = 0x01,
      };

      /**
       * @brief Codes for LoRa channel.
       *
       *  Channels allowed are 10 to 17 for 868 MHz band and 0 to 12 for 900 MHZ band.
       *
       */
      enum _FREQUENCY_CHANNEL
      {
        /// Undefined or not valid channel
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
       * @brief Codes for bandwidth.
       *
       *  Bandwidth allowed are 125, 250 and 500 KHz.
       *
       */
      enum _BANDWIDTH
      {
        /// Undefined or not valid bandwidth
        BW_UNKN = 0xFF,

        /// Bandwidth 125 KHz
        BW_125 = 0x00,

        /// Bandwidth 250 KHz
        BW_250 = 0x01,

        /// Bandwidth 500 KHz
        BW_500 = 0x02,
      };

      /**
       * @brief Codes for the coding rate.
       *
       *  Coding rate is a number between 5 and 8.
       *
       */
      enum _CODING_RATE
      {
        /// Undefined or not valid coding rate
        CR_UNKN = 0xFF,

        /// Coding rate 5
        CR_5 = 0x00,

        /// Coding rate 6
        CR_6 = 0x01,

        /// Coding rate 7
        CR_7 = 0x02,

        /// Coding rate 8
        CR_8 = 0x03,
      };

      /**
       * @brief Codes for the spreading factor.
       *
       *  Spreading factor is a number between 6 and 12.
       *
       */
      enum _SPREADING_FACTOR
      {
        /// Undefined or not valid spreading factor
        SF_UNKN = 0xFF,

        /// Spreading factor 6
        SF_6 = 0x00,

        /// Spreading factor 7
        SF_7 = 0x01,

        /// Spreading factor 8
        SF_8 = 0x02,

        /// Spreading factor 9
        SF_9 = 0x03,

        /// Spreading factor 10
        SF_10 = 0x04,

        /// Spreading factor 11
        SF_11 = 0x05,

        /// Spreading factor 12
        SF_12 = 0x06,
      };

      /**
       * @brief Creates an empty configuration command.
       *
       */
      ConfigCommand();

      /**
       * @brief Copy constructor.
       *
       * @param[in] cmd configuration command object.
       *
       */
      ConfigCommand(const ConfigCommand &cmd);

      /**
       * @brief Destroys a configuration command.
       *
       */
      virtual ~ConfigCommand();

      /**
       * @brief Sets frequency band to a specific frequency code.
       *
       * This function sets the frequency band selecting it using a frequency
       * code, according to lora::ConfigCommand::_FREQUENCY enum.
       * Frequency bands allowed are 868 MHz (Europe) and 900 MHz (US).
       *
       * @param[in] freq frequency code.
       *
       * @returns false if frequency value is not allowed, true otherwise.
       */
      bool setFrequency(uint8_t freq);

      /**
       * @brief Gets the frequency band.
       *
       * This function returns the frequency band. If the parameter code is true
       * frequency code (lora::ConfigCommand::_FREQUENCY) is returned else
       * function return the frequency band in MHz.
       *
       * @param[in] code flag to select the type of value to return.
       *
       * @returns frequency code if code is true otherwise frequency band in MHz.
       */
      uint16_t frequency(bool code = true);

      /**
       * @brief Gets the frequency band as a string.
       *
       * This function returns the frequency band in MHz as a std::string.
       *
       * @returns frequency band in MHz.
       */
      std::string frequencyAsString();

      /**
       * @brief Sets channel to a specific channel code.
       *
       * This function sets the channel selecting it using a channel
       * code, according to lora::ConfigCommand::_FREQUENCY_CHANNEL enum.
       * Channels allowed are 10 to 17 for 868 MHz band and 00 to 12 for
       * 900 MHz band. Other channel values will be considered as unknown
       * and not valid.
       *
       * @param[in] ch channel code.
       *
       * @returns false if channel value is not allowed, true otherwise.
       */
      bool setChannel(uint8_t ch);

      /**
       * @brief Gets the channel value
       *
       * This function returns the LoRa channel. If the parameter code is true
       * channel label (defined in
       * lora::ConfigCommand::_FREQUENCY_CHANNEL enum) is returned
       * else function return the channel number.
       *
       * @param[in] code flag to select the type of value to return.
       *
       * @returns channel label if code is true otherwise the channel number.
       */
      uint8_t channel(bool code = true);

      /**
       * @brief Gets the channel number as a string.
       *
       * This function returns the channel number as a std::string.
       *
       * @returns channel number.
       */
      std::string channelAsString();

      /**
       * @brief Sets the node address.
       *
       * This function sets the node address. It is a number between 1 and
       * 255 (0 is for broadcast message).
       *
       * @param[in] addr node address.
       *
       * @returns false if node address value is valid, true otherwise.
       */
      inline bool setAddress(uint8_t addr)
      {
        m_addr = addr;
        return (m_addr > 0);
      }

      /**
       * @brief Gets the node address.
       *
       * This function returns the node address.
       *
       * @returns node address.
       */
      inline uint8_t address()
      {
        return m_addr;
      }

      /**
       * @brief Sets the channel bandwidth to a specific bandwidth code.
       *
       * This function sets the  channel bandwidth selecting it using a
       * code, according to lora::ConfigCommand::_BANDWIDTH enum.
       * Bandwidth can be 125, 250 or 500 KHz.
       *
       * @param[in] bw bandwidth code.
       *
       * @returns false if bandwidth code is not allowed, true otherwise.
       */
      bool setBandwidth(uint8_t bw);

      /**
       * @brief Gets the bandwidth value.
       *
       * This function returns the channel bandwidth. If the parameter code is true
       * bandwidth label (defined in
       * lora::ConfigCommand::_FREQUENCY_CHANNEL enum) is returned
       * else function return the bandwidth value in KHz.
       *
       * @param[in] code flag to select the type of value to return.
       *
       * @returns bandwidth label if code is true otherwise the bandwidth value in KHz.
       */
      uint16_t bandwidth(bool code = true);

      /**
       * @brief Gets the bandwidth as a printable string.
       *
       * This function returns the bandwidth value (in KHz) as a std::string.
       *
       * @returns bandwidth number.
       */
      std::string bandwidthAsString();

      /**
       * @brief Sets coding-rate to a specific code.
       *
       * This function sets the coding-rate selecting it using a
       * code, according to lora::ConfigCommand::_CODING_RATE enum.
       * Coding rate is a number between 5 and 8. Other channel values
       * will be considered as unknown and not valid.
       *
       * @param[in] cr coding-rate.
       *
       * @returns false if coding-rate value is not allowed, true otherwise.
       */
      bool setCodingRate(uint8_t cr);

      /**
       * @brief Gets the coding-rate value.
       *
       * This function returns the coding-rate. If the parameter code is true
       * coding-rate label (defined in
       * lora::ConfigCommand::_CODING_RATE enum) is returned
       * else function return the coding-rate number.
       *
       * @param[in] code flag to select the type of value to return.
       *
       * @returns coding-rate label if code is true otherwise the channel number.
       */
      uint8_t codingRate(bool code = true);

      /**
       * @brief Gets the coding-rate as a printable string.
       *
       * This function returns the coding-rate value as a std::string.
       *
       * @returns coding-rate value.
       */
      std::string codingRateAsString();


      /**
       * @brief Sets spreading factor to a specific code.
       *
       * This function sets the spreading factor selecting it using a
       * code, according to lora::ConfigCommand::_SPREADING_FACTOR enum.
       * Coding rate is a number between 6 and 12. Other channel values
       * will be considered as unknown and not valid.
       *
       * @param[in] sf spreading factor.
       *
       * @returns false if spreading factor value is not allowed, true otherwise.
       */
      bool setSpreadingFactor(uint8_t sf);

      /**
       * @brief Gets the spreading factor value.
       *
       * This function returns  the spreading factor value. If code parameter
       * is true, function returns the spreading factor code according to
       * lora::ConfigCommand::_SPREADING_FACTOR enum, otherwise it returns the
       * spreading factor value.
       *
       * @param[in] code flag to select the type of value to return.
       *
       * @returns spreading factor label if code is true otherwise the spreading factor number.
       */
      uint8_t spreadingFactor(bool code = true);

      /**
       * @brief Gets the spreading factor as a printable string.
       *
       * This function returns the spreading factor value as a std::string.
       *
       * @returns spreading factor value.
       */
      std::string spreadingFactorAsString();

      /**
       * @brief Gets the RSSI.
       *
       * This function returns the RSSI.
       *
       * @returns RSSI value.
       */
      inline int rssi()
      {
        return m_rssi;
      }

      /**
       * @brief Gets the SNR.
       *
       * This function returns the SNR.
       *
       * @returns SNR value.
       */
      inline int snr()
      {
        return m_snr;
      }

      /**
       * @brief Gets the packet RSSI.
       *
       * This function returns the packet RSSI.
       *
       * @returns packet RSSI value.
       */
      inline int rssi_pck()
      {
        return m_rssi_pck;
      }

    protected:
      //! LoRa frequency band
      uint8_t m_freq;

      //! LoRa channel
      uint8_t m_ch;

      //! Node address
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
