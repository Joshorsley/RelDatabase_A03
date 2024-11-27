#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define SIZE_DATE_STRING 11
#define SIZE_BUFFER 500

#define SUCCESS 0
#define ERR_INPUT_ERROR 1
#define ERR_INVALID_INPUT 2
#define ERR_INVALID_BUFFER 3


//Gets an int from console
int GetInt(int* rValue);
//Gets a string from console 
    //Must provide size of char* rValue in the bufferSize
int GetString(char* rValue, size_t bufferSize);

//Expects a string of the EXACT FORMAT:
//"YYYY-MM-DD", else returns error.
//This one's sloppy, feel free to fix.
int GetDate(char* rValue, size_t bufferSize);

int GetInt(int* rvalue) {
    char buffer[SIZE_BUFFER];
    
    //Get line
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return ERR_INPUT_ERROR;//No input
    }

    //Points to first char after int parsed
    char* endPtr;
    //Try to parse int
    int temp = strtol(buffer, &endPtr, 10);
    
    //Check first char after int was end of string
    if (*endPtr != '\n' && *endPtr != '\0') {
        return ERR_INVALID_INPUT; //Input was not a valid integer
    }

    //Success
    *rvalue = temp;
    return SUCCESS;
}


int GetString(char* rValue, size_t bufferSize) {
    char buffer[SIZE_BUFFER];
    if (bufferSize > SIZE_BUFFER) {
        return ERR_INVALID_BUFFER; //Get outta here, you don't need 500 chars.
    }

    //Get line
    if (fgets(buffer, bufferSize, stdin) == NULL) {
        return ERR_INPUT_ERROR;//No input
    }

    // Remove trailing newline
    size_t len = strlen(buffer);

    if (buffer[len - 1] != '\n') {
        return ERR_INPUT_ERROR;//Newline not where it's supposed to be
        //User probably exceeded buffer size.
    }

    buffer[len - 1] = '\0';

    strcpy(rValue, buffer);
    return SUCCESS; // Success
}

int GetDate(char* rValue, size_t bufferSize) {
    char* buffer[SIZE_BUFFER];
    
    if (bufferSize != SIZE_DATE_STRING) {
        return ERR_INVALID_BUFFER;//Wrong buffer size.
    }
    printf("Enter a date <YYYY-MM-DD> WITH '-' chars: ");
    if (fgets(buffer, SIZE_DATE_STRING, stdin) == NULL) {
        return ERR_INPUT_ERROR;//Input error
    }

    if (buffer[SIZE_DATE_STRING - 1] != '\n') {
        return ERR_INVALID_INPUT;//Bad format.
    }
    if (buffer[4] != '-' || buffer[7] != '-') {
        return ERR_INVALID_INPUT;
    }

    strcpy(rValue, buffer);
    return SUCCESS; // Success
}
