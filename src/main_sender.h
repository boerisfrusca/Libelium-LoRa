//============================================================================
// Name        : main_sender.h
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Header of the main for the "Lo-Ra sender"
//============================================================================
#ifndef MAIN_SENDER_H_
#define MAIN_SENDER_H_

//#define LORA_SENDER
#ifdef LORA_SENDER

/*****************************************************************************
 * MACROS
 ****************************************************************************/
#ifndef LORA_NAME
#define LORA_NAME             "lora_sender"
#define LORA_VERSION          "1.0"
#endif

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
 * @brief Main function for the Lo-Ra sender.
 *
 * @param[in] argc number of strings pointed to by argv
 * @param[in] argv arguments vector
 *
 * @returns 0 (false) if errors, 1 (true) otherwise.
 */
int main_sender(int argc, char **argv);

#endif

#endif /* MAIN_SENDER_H_ */
