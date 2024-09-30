#ifndef AUXILIARY_H
#define AUXILIARY_H

#include "pexec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef const char *error_t;

error_t fopen_home(FILE **_fp, char const _f[], char const _m[]);
int     popen_ssh(FILE **_fp0, FILE **_fp1, PEXEC_PID *_pid); /* 0: success, -1: failure */
int     read_prompt(PEXEC_PID pid, FILE *_fp0, FILE *_fp1, char const *_step); /* 0: success, -1: failure */

#endif
