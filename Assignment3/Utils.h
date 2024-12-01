#pragma once
// Utils.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable: 4996)


#define SIZE_DATE_STRING 12
#define SIZE_BUFFER 500

#define SUCCESS 0
#define ERR_NO_INPUT 1
#define ERR_INVALID_INPUT 2
#define ERR_INVALID_BUFFER 3

int GetInt(int* rValue);
int GetString(char* rValue, size_t bufferSize);
int GetDate(char* rValue, size_t bufferSize);
int ValidateEmail(char* rValue);
