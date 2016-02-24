//============================================================================
// Name        : main_test.h
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Header of the main for the "Lo-Ra configuration reader"
//============================================================================
#ifndef MAIN_TEST_H_
#define MAIN_TEST_H_

//#define LORA_TEST
#ifdef LORA_TEST
/*****************************************************************************
 * MACROS
 ****************************************************************************/

#ifndef LORA_NAME
  #define LORA_NAME             "main_config"
  #define LORA_VERSION          "1.0"
#endif

/*****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
/**
 * @brief Prints the help message of the command.
 *
 * This function prints on standard error the help of the \elora_config
 * command.
 *
 */
void print_help(void);

/**
 * @brief Main function for the Lo-Ra configuration reader.
 *
 * @param[in] argc number of strings pointed to by argv
 * @param[in] argv arguments vector
 *
 * @returns 0 (false) if errors, 1 (true) otherwise.
 */
int main_test(int argc, char **argv);


#endif


#endif /* MAIN_TEST_H_ */
