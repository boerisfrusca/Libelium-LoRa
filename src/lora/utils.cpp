//============================================================================
// Name        : utils.cpp
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Functions for generic purpose
//============================================================================

#include "utils.h"

uint8_t convertHexToChar(uint8_t val)
{
  if (val < 10)
  {
    return ('0' + val);
  }
  else
  {
    return ('A' + (val - 10));
  }

  return 0;
}

void convertHexToChars(uint8_t val, uint8_t &high, uint8_t &low)
{
  uint8_t nh = (val & 0xF0) >> 4; // High nibble
  uint8_t nl = val & 0x0F;        // Low nibble

  high = convertHexToChar(nh);
  low = convertHexToChar(nl);
}


void convertIntToSting(int val, std::string &str)
{
  std::ostringstream convert;   // stream used for the conversion
  convert << val;               // insert the textual representation of 'Number' in the characters in the stream
  str = convert.str();          // set 'Result' to the contents of the stream
}


uint8_t convertHexCharToInt(uint8_t val)
{
  uint8_t c = 0;

  if (val >= '0' && val <= '9')
  {
    c = val - '0';
  }
  else if (toupper(val) >= 'A' && toupper(val) <= 'F')
  {
    c = toupper(val) - 'A' + 10;
  }
  else
  {
    val = 0x0FF;
  }

  return c;
}


void toUpper(std::string &str)
{
  for (std::string::iterator it = str.begin(); it != str.end(); ++it)
    *it = toupper(*it);
}

void toLower(std::string &str)
{
  for (std::string::iterator it = str.begin(); it != str.end(); ++it)
    *it = tolower(*it);
}

bool is_number(const std::string &str)
{
  for (size_t i = 0; i < (str.size() - 1); i++)
  {
    if ( !isdigit(str[i]) )
    {
      if ( i == 0 && str[i] == '-')
      {
      }
      else
        return false;
    }
  }
  return true;
}

