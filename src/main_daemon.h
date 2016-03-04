//============================================================================
// Name        : main_daemon.h
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Header of the main for the "Lo-Ra daemon"
//============================================================================
#ifndef MAIN_DAEMON_H_
#define MAIN_DAEMON_H_

//#define LORA_DAEMON
#ifdef LORA_DAEMON

/*****************************************************************************
 * MACROS
 ****************************************************************************/
#ifndef LORA_NAME
#define LORA_NAME             "lora_daemon"
#define LORA_VERSION          "1.0"
#endif

#define PIPE_NAME "/tmp/lora.pipe"

#include "circularbuffer.h"
/**
 * Data buffer.
 */
typedef CircularBuffer<uint8_t> Buffer;

/*****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
/**
 * @brief Prints the help message of the command.
 *
 * This function prints on standard error the help of the \elora_sender
 * command.
 *
 */
void print_help(void);

/**
 * @brief Main function for the Lo-Ra daemon.
 *
 * This command opens a serial connection with the LoRa gateway and
 * transmits messages received from a pipe /tmp/lora
 *
 * @param[in] argc number of strings pointed to by argv
 * @param[in] argv arguments vector
 *
 * @returns 0 (false) if errors, 1 (true) otherwise.
 */
int main_daemon(int argc, char **argv);

/**
 * Function called when a signal is caught.
 *
 * \param[out] signum signal number.
 *
 */
void signalCallbackHandler(int signum);

/**
 * Check if a file exists
 * @return true if and only if the file exists, false else
 */
bool fileExists(const char* file);

uint8_t createDataCommand(uint8_t *buffer, uint8_t dest, std::string &msg);

#endif

#endif /* MAIN_DAEMON_H_ */
