/**
 * @file pexec.h
 * @brief This file provide a UNIX/Windows compatible piped command launcher. 
 *
 * @date 2024/09/30
 * @bug No known bugs.
 * @license OpenBSD ISC
 */
#ifndef PEXEC_H
#define PEXEC_H
#include <stdio.h>
#ifdef __unix__
#  define PEXEC_PID int
#  define PEXEC_INVALID (-1)
#endif
#ifdef _WIN32
#  include <windows.h>
#  define PEXEC_PID HANDLE
#  define PEXEC_INVALID INVALID_HANDLE_VALUE
#endif
#ifndef PEXEC_PID
#  error Unsupported platform.
#endif

typedef char const *error_t;

error_t pexec(char const *_cmd[], FILE **_fp0, FILE **_fp1, PEXEC_PID *_pid);
error_t pwait(PEXEC_PID _pid);
error_t pkill(PEXEC_PID _pid);

#endif
