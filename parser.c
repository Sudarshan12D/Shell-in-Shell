/*
* File Name: parser.c
* Assignment: Lab 3
* Completed by: Sudarshan Naicker, Alessandro Baldassarre
* Submission Date: Oct 04, 2023
*/

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"

// Command to trim whitespace and ASCII control characters from buffer
//[Input] char* inputbuffer - input string to trim
//[Input] size_t bufferlen - size of input and output string buffers
//[Output] char* outputbuffer - output string after trimming
//[Return] size_t - size of output string after trimming
size_t trimstring(char *outputbuffer, const char *inputbuffer, size_t bufferlen)
{
    memcpy(outputbuffer, inputbuffer, bufferlen * sizeof(char));

    for (size_t ii = strlen(outputbuffer) - 1; ii >= 0; ii--)
    {
        if (outputbuffer[ii] < '!') // In ASCII '!' is the first printable (non-control) character
        {
            outputbuffer[ii] = 0;
        }
        else
        {
            break;
        }
    }

    return strlen(outputbuffer);
}


int tokenize_command(char *parsedinput, char *args[])
{
    int i = 0;
    char *start = parsedinput;
    while (*parsedinput != '\0' && i < 19)
    {
        if (*parsedinput == '"')
        {
            parsedinput++; // Move past the opening quote
            start = parsedinput;
            while (*parsedinput != '"' && *parsedinput != '\0')
            {
                parsedinput++;
            }
            if (*parsedinput == '\0')
            {
                fprintf(stderr, "Missing closing quote for argument: %s\n", start);
                return -1;
            }
            *parsedinput = '\0'; // Replace closing quote with null terminator
            args[i++] = start;
        }
        else if (*parsedinput != ' ')
        {
            start = parsedinput;
            while (*parsedinput != ' ' && *parsedinput != '\0')
            {
                parsedinput++;
            }
            if (*parsedinput != '\0')
            {
                *parsedinput = '\0'; // Replace space with null terminator
            }
            args[i++] = start;
        }
        parsedinput++;
        
    }

    
    args[i] = NULL;
    

    
    
    return 0;
}



// Command to trim the input command to just be the first word
//[Input] char* inputbuffer - input string to trim
//[Input] size_t bufferlen - size of input and output string buffers
//[Output] char* outputbuffer - output string after trimming
//[Return] size_t - size of output string after trimming
size_t firstword(char *outputbuffer, const char *inputbuffer, size_t bufferlen)
{
    size_t length = strlen(inputbuffer);
    size_t i;

    // Find the first space or end of string
    for (i = 0; i < length && inputbuffer[i] != ' '; i++)
    {
        if (i < bufferlen - 1)
        {
            outputbuffer[i] = inputbuffer[i];
        }
        else
        {
            break; // Output buffer is full, stop copying
        }
    }

    // Ensure the output string is null-terminated
    if (i < bufferlen - 1)
    {
        outputbuffer[i] = '\0';
    }
    else
    {
        outputbuffer[bufferlen - 1] = '\0'; // Null-terminate even if buffer is full
    }

    return i;
}

// Command to test that string only contains valid ascii characters (non-control and not extended)
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] bool - true if no invalid ASCII characters present
bool isvalidascii(const char *inputbuffer, size_t bufferlen)
{
    // TO DO: Correct this function so that the second test string fails
    size_t testlen = bufferlen;
    size_t stringlength = strlen(inputbuffer);
    if (strlen(inputbuffer) < bufferlen)
    {
        testlen = stringlength;
    }

    for (size_t ii = 0; ii < testlen; ii++)
    {
        if ((unsigned char)inputbuffer[ii] < 0x20 || (unsigned char)inputbuffer[ii] > 0x7E)
        {
            return false;
        }
    }

    return true;
}

// Command to find location of pipe character in input string
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] int - location in the string of the pipe character, or -1 pipe character not found
int findpipe(const char *inputbuffer, size_t bufferlen)
{
    // TO DO: Implement this function
    const char* pipePos = strchr(inputbuffer, '|');
    if (pipePos != NULL) {
        return (int)(pipePos - inputbuffer);
    } else {
        return -1;
    }

    
}

// Command to test whether the input string ends with "&" and
// thus represents a command that should be run in background
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] bool - true if string ends with "&"
bool runinbackground(const char *inputbuffer, size_t bufferlen)
{
    size_t input_length = strlen(inputbuffer);

    // Ensure buffer length is at least 1 to avoid invalid access
    if (input_length > 0)
    {
        return (inputbuffer[input_length - 1] == '&');
    }

    return false;
}


char *find_command_in_path(const char *command)
{
    char *path = strdup(getenv("PATH"));
    if (!path) {
        return NULL; 
    }
    char *token = strtok(path, ":");
    static char cmd_path[BUFLEN]; // Made it static to safely return it

    while (token != NULL)
    {
        snprintf(cmd_path, sizeof(cmd_path), "%s/%s", token, command);

        // Check if the file exists and is executable
        if (access(cmd_path, X_OK) == 0)
        {
            free(path);
            return cmd_path; // Return the found command path
        }

        token = strtok(NULL, ":");
    }
    
    free(path);
    return NULL;
}

