//============================================================================
// Name        : utils.h
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Functions for generic purpose
//============================================================================
#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>
#include <sstream>
#include <iomanip>

/**
 * Converts a number of one byte in 2 characters which contain the hexadecimal value.
 *
 * \param[in] val byte to converter
 * \param[out] high high nibble
 * \param[out] low low nibble
 *
 */
void convertHexToChars(uint8_t val, uint8_t &high, uint8_t &low);

/**
 * Converts a nibble into a character with its hexadecimal value.
 *
 * \param[in] val byte with the nibble to converter
 *
 * \return character with its hexadecimal value.
 */
uint8_t convertHexToChar(uint8_t val);

/**
 * Converts an integer value into a string.
 *
 * \param[in] val value to converter
 * \param[out] str string which contains the converted value
 *
 */
void convertIntToSting(int val, std::string &str);

/**
 * Converts a character with an hexadecimal digit into a number.
 *
 * \param[in] val character with an hexadecimal digit
 *
 * * \return byte with the converted value.
 */
uint8_t convertHexCharToInt(uint8_t val);

/**
 * Converts all characters of a string for lower to upper case.
 *
 * \param[out] str string to convert.
 */
void toUpper(std::string &str);


/**
 * Converts all characters of a string for upper to lower case.
 *
 * \param[out] str string to convert.
 */
void toLower(std::string &str);

/**
 * Checks if a string is a number
 *
 * \param[in] str string to test.
 *
 * \return true if string contains a number, folase otherwise.
 */
bool is_number(const std::string &str);

#endif /* UTILS_H_ */
