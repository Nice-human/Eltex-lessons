#ifndef INTRFACE_H
#define INTRFACE_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <locale.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

void remove_newline(char *str);
void parse_arguments(char *str, char **args);
char* trim (char* str);
void execute_simple_command(char* input);
void execute_pipe_command(char* input, char* pipe_pos);
#endif