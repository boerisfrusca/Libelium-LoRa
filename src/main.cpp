//============================================================================
// Name        : main.cpp
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Main for Waspmote Lo-Ra tools
//============================================================================
#include <iostream>
#include <string>
#include <cstring>
#include <stdarg.h>
#include "global.h"
#include "verbose.h"
#include "lora/utils.h"
#include "lora/serial.h"
#include "lora/command.h"

#ifdef LORA_SENDER
#include "main_sender.h"
#else
#ifdef LORA_TEST
#include "main_test.h"
#endif
#ifdef LORA_SETUP
#include "main_setup.h"
#endif
#endif

/*************************************************************************
 * MACROS
 ************************************************************************/
#ifndef LORA_NAME
  #define LORA_NAME             "lora_sender"
  #define LORA_VERSION          "1.0"
#endif

/*************************************************************************
 * TYPE AND ENUM DEFINITONS
 ************************************************************************/


/*************************************************************************
 * GLOABALS
 ************************************************************************/


/*************************************************************************
 * MAIN
 ************************************************************************/
int main(int argc, char **argv)
{
  int ret = 0;
#ifdef LORA_SENDER
  ret = main_sender(argc, argv);
#endif
#ifdef LORA_TEST
  ret = main_test(argc, argv);
#endif
#ifdef LORA_SETUP
  ret = main_setup(argc, argv);
#endif
  return ret;
}
