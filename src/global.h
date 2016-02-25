//============================================================================
// Name        : global.h
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Header for global functions and constants
//============================================================================
#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <iostream>
#include <string>
#include <cstring>
#include <stdarg.h>
#include "verbose.h"
#include "lora/utils.h"
#include "lora/serial.h"
#include "lora/command.h"

/*****************************************************************************
 * MACROS
 ****************************************************************************/
#define SERIAL_DEVICE    "/dev/ttyUSB0"             // Default serial device
#define SERIAL_BITRATE   38400                      // Default bitrate of the
                                                    // serial device

#define TX_NODE          1                           // Default TX node addr
#define TX_FREC          lora::ConfigCommand::F_868  // 868 MHz
#define TX_CH            lora::ConfigCommand::CH_10  // Channel 10
#define TX_BW            lora::ConfigCommand::BW_125 // 125 KHz
#define TX_CR            lora::ConfigCommand::CR_5   // Coding rate 6
#define TX_SF            lora::ConfigCommand::SF_6   // Spreading factor 6

#define RX_NODE          2                           // Default RX node addr

#define FLUSH_TIMEOUT    5                           // Time of reading at
                                                     // the start (sec)
#define RX_TIMEOUT       100                         // Receive timeout (sec)

/*****************************************************************************
 * TYPE AND ENUM DEFINITONS
 ****************************************************************************/
/**
 * @brief Codes for Lo-Ra command.
 *
 * Internal code to identify a Lo-Ra command implemented by the Libelium SX1272
 * LoRa module. The commands used for the module are:
 *
 *   - \bREAD  : gets the current setup of the module.
 *   - \bSET   : sets a new configuration in the module.
 *   - \bDATA  : sends data from the module to a Waspmote.
 *   - \bERROR : notifies an error in the information received.
 *   - \bINFO  : with this command, the modules gives its setup.
 *   - \bACK   : with this command, the modules notifies that a message has been successfully received.
 *
 */
enum _COMMAND
{
  /// Unknown command or invalid command code
  CMD_UNKNOWN = 0,

  /// READ command
  CMD_READ = 1,

  /// SET command
  CMD_SET = 2,


  /// DATA command
  CMD_DATA = 3,

  /// ACK command
  CMD_ACK = 4,

  /// INFO command
  CMD_INFO = 5,

  /// ERROR command
  CMD_ERROR = 6
};

/// Default maximum buffer size
const uint8_t buf_sz = 255;


/*****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
/**
 * @brief Opens a serial device.
 *
 * This function opens a serial connection using a device described by
 * lora::Serial class.
 *
 * @param[out] serial serial device.
 *
 * @returns 0 (false) if open operation failed, true otherwise.
 */
bool openSerial(lora::Serial &serial);

/**
 * @brief Closes a serial device.
 *
 * This function closes a serial connection handled by the device described
 * by lora::Serial class.
 *
 * @param[out] serial serial device.
 *
 * @returns 0 (false) if close operation failed, true otherwise.
 */
bool closeSerial(lora::Serial &serial);

/**
 * @brief Converts a Waspmote command in a printable string.
 *
 * Waspmote commands are a binary stream with the following structure:
 *
 * | <SOH> | <COMMAND> | [#] | [DATA] | <CR> | <LF> | CRC16 | <EOT> |
 *
 * where <SOH>, <CR>, <LF> and <EOT> are special characters.
 * This function converts special characters in printable sequences and
 * saves the message in a std::string.
 *
 * @param[in] buffer buffer where command is stored.
 * @param[in] sz buffer size.
 *
 * @returns a std::string with the command.
 */
std::string msg_string(uint8_t *buffer, size_t sz);

/**
 * @brief Processes all bytes of buffer looking for a Lo-Ra command.
 *
 * This function is used by all tool and prints the information of the
 * command, if it is found in the buffer.
 *
 * @param[in] buffer buffer of byte to process.
 * @param[in] sz buffer size.
 *
 */
void process_buffer(uint8_t *rx_buffer, size_t sz);

/**
 * @brief Reads all bytes present into the serial receive buffer.
 *
 * This function empty the receive buffer of the serial device reading from
 * the serial device for a fixed time (5 seconds) .
 *
 * @param[out] serial serial device.
 *
 */
void rx_buffer_flush (lora::Serial &serial);
#endif /* GLOBAL_H_ */
