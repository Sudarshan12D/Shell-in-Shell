/*
* File Name: parser.h
* Assignment: Lab 3
* Completed by: Sudarshan Naicker, Alessandro Baldassarre
* Submission Date: Oct 04, 2023
*/

#ifndef __PARSER_H
#define __PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

#define BUFLEN 1024


size_t trimstring(char* outputbuffer, const char* inputbuffer, size_t bufferlen);
size_t firstword(char* outputbuffer, const char* inputbuffer, size_t bufferlen);
int tokenize_command(char *parsedinput, char *args[]);
bool isvalidascii(const char* inputbuffer, size_t bufferlen);
char *find_command_in_path(const char *command);
int findpipe(const char* inputbuffer, size_t bufferlen);
bool runinbackground(const char* inputbuffer, size_t bufferlen);

#endif