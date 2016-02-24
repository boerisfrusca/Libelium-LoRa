//============================================================================
// Name        : main_setup.h
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Header of the main for the "Lo-Ra configuration writer"
//============================================================================

#ifndef MAIN_SETUP_H_
#define MAIN_SETUP_H_

//#define LORA_SETUP
#ifdef LORA_SETUP

/*************************************************************************
 * MACROS
 ************************************************************************/
#ifndef LORA_NAME
#define LORA_NAME             "lora_setup"
#define LORA_VERSION          "1.0"
#endif

/**
 * @brief Prints the help message of the command.
 *
 * This function prints on standard error the help of the \elora_setup command.
 *
 */
void print_help(void);

/**
 * @brief Main function for the Lo-Ra configuration writer.
 *
 * @param[in] argc number of strings pointed to by argv
 * @param[in] argv arguments vector
 *
 * @returns 0 (false) if errors, 1 (true) otherwise.
 */
int main_setup(int argc, char **argv);


#endif


#endif /* MAIN_SETUP_H_ */
