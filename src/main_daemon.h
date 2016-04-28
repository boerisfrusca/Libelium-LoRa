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

/**
 * @brief parameter for the 'write' thread
 */
typedef struct _tx_param{
    /// Address of the destination node
    uint8_t dest;

    /// Minimum time between two write operation
    uint8_t timeout;

    /// Pointer to the pipe path
    std::string *pipe;

    /// Pointer to the error code
    uint8_t error;

    /// Pointer to the serial connection
    lora::Serial *serial;
} tx_param;

/**
 * @brief parameter for the 'read' thread
 */
typedef struct _rx_param{
    /// Pointer to the error code
    uint8_t error;

    /// Pointer to the serial connection
    lora::Serial *serial;
} rx_param;

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
 * @brief Function for the thread that writes to the serial device.
 *
 * This function is the core of the 'write' thread. It reads messages from
 * the pipe, creates LoRa packet and then writes them to the serial device.
 *
 * @param[out] arg pointer to the function parameter (@tx_param type).
 *
 * \return a void pointer.
 */
void* t_write_function(void *arg);

/**
 * @brief Function for the thread that reads to the serial device.
 *
 * This function is the core of the 'read' thread. It reads a byte stream
 * from the serial port, looks for a LoRa packet and processes it.
 *
 * @param[out] arg pointer to the function parameter (@rx_param type).
 *
 * \return a void pointer.
 */
void* t_read_function(void *arg);

/**
 * @brief Checks if a file exists
 *
 * @param file absolute path of the file
 *
 * @return true if and only if the file exists, false else
 */
bool fileExists(const char* file);

/**
 * @brief Creates a LoRa command of type DATA.
 *
 * This function creates a DATA command.
 *
 * @param[out] buffer DATA command created by the function
 * @param[in] dest destination address
 * @param[in] msg message
 */
uint8_t createDataCommand(uint8_t *buffer, uint8_t dest, std::string &msg);

#endif

#endif /* MAIN_DAEMON_H_ */
