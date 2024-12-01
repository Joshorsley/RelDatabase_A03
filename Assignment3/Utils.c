#include "Utils.h"

//Gets an int from console
int GetInt(int* rValue);
//Gets a string from console 
    //Must provide size of char* rValue in the bufferSize
int GetString(char* rValue, size_t bufferSize);

//Expects a string of the EXACT FORMAT:
//"YYYY-MM-DD", else returns error.
//This one's sloppy, feel free to fix.
int GetDate(char* rValue, size_t bufferSize);

//Checks if the string passed is in correct email format
int ValidateEmail(char* rValue);

int GetInt(int* rvalue) {
    char buffer[SIZE_BUFFER];
    
    //Get line
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return ERR_NO_INPUT;//No input
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
        return ERR_NO_INPUT;//No input
    }

    // Remove trailing newline
    size_t len = strlen(buffer);

    if (buffer[len - 1] != '\n') {
        return ERR_NO_INPUT;//Newline not where it's supposed to be
        //User probably exceeded buffer size.
    }

    buffer[len - 1] = '\0';
    FlushBuffer();


    strcpy(rValue, buffer);
    return SUCCESS; // Success
}

int GetDate(char* rValue, size_t bufferSize) {
    char buffer[SIZE_BUFFER];
    
    if (bufferSize != SIZE_DATE_STRING) {
        return ERR_INVALID_BUFFER;//Wrong buffer size.
    }
    if (fgets(buffer, bufferSize, stdin) == NULL) {
        return ERR_NO_INPUT;//Input error
    }

    if (buffer[SIZE_DATE_STRING - 1] != '\0') {
        return ERR_INVALID_INPUT;//Bad format.
    }
    if (buffer[4] != '-' || buffer[7] != '-') {
        return ERR_INVALID_INPUT;
    }
    FlushBuffer();

    strcpy(rValue, buffer);
    return SUCCESS; // Success
}

int ValidateEmail(char* rValue)
{
    int atCounter = 0;
    int periodCounter = 0;
    int atPosition = 0;
    int periodPosition = 0;
    int charsBetween = 0;

    for (int i = 0; i < strlen(rValue); i++)
    {
        if (rValue[i] == '@')
        {
            atCounter++;
            atPosition = i;
        }
        if (rValue[i] == '.')
        {
            periodCounter++;
            periodPosition = i;
        }
    }

    charsBetween = periodPosition - atPosition;  //The number of characters between the @ and the .

    /*There must be 1 @ , at least 1 . , at least 1 character before the @, the . must come after the @ ,
    there must be more than 1 character between the @ and . */
    if (atCounter != 1 || periodCounter == 0 || atPosition < 1 || atPosition > periodPosition || charsBetween == 1)
    {
        return ERR_INVALID_INPUT;
    }

    charsBetween = strlen(rValue) - periodPosition; //The number of characters between the end of the email and the final .

    if (charsBetween == 1) //There must be at least 1 character between the final . and the end of the email
    {
        return ERR_INVALID_INPUT;
    }

    return SUCCESS;
}


void FlushBuffer() {
    int c;
    //Consume chars until \n or EOF
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}