//============================================================================
// Name        : verbose.h
// Author      : Marco Boeris Frusca and Ferdinando Ricchiuti
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Header for functions to handle the verbose output
//============================================================================
#ifndef VERBOSE_H_
#define VERBOSE_H_

#include <time.h>
#include <stdint.h>

/*****************************************************************************
 * MACROS
 ****************************************************************************/
/**
 * @brief Macro used to print an error message.
 */
#define V_ERROR(...) v_log(0, __FILE__, __LINE__, __func__,  __VA_ARGS__)

/**
 * @brief Macro used to print an information message.
 */
#define V_INFO(...)  v_log(1, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * @brief Macro used to print a debug message.
 */
#define V_DEBUG(...) v_log(2, __FILE__, __LINE__, __func__,  __VA_ARGS__)

/**
 * @brief True if the level required is up to information.
 */
#define V_INFO_REQUIRED v_required_by_verbosity(1)

/**
 * @brief True if the level required is up to debug.
 */
#define V_DEBUG_REQUIRED v_required_by_verbosity(2)

/*****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
/**
 * @brief Writes a log message on standard error.
 *
 * Users should call this function using a macro, becasue
 * in this way the __LINE__ and __FILE__ precompiler macros
 * can be used to reference the piece of code that has generated
 * the message.
 *
 * @param[in] level the level of the message (0 an important)
 * @param[in] file the source file name that as fired the message
 * @param[in] line the source file line that as fired the message
 * @param[in] func the function name
 * @param[in] fmt the same as @see{printf}
 */
void v_log(int level, char *file, int line, const char *func, char *fmt, ...);

/**
 * @brief Sets the verbosity level.
 *
 * The verbosity level affects @see{tempo2_log}. Only messages
 * having level <= of the vervosity level will be printed.
 *
 * @param[in] level verbosity level
 */
void v_verbosity(int level);

/**
 * @brief Returns true if the given level is required by verbosity.
 *
 * This function may be used to decide if a function or a block of
 * code must be called. Some pieces of code is only there to provide
 * debug information ad is not required for the actual verbosity level.
 *
 * @param[in] level verbosiy level
 * @returns 0 (false) if level > verbosity level
 */
int v_required_by_verbosity(int level);

#endif /* VERBOSE_H_ */
