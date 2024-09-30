/**
 * @file pexec.c
 * @brief This file implements "pexec.h" for UNIX machines. 
 *
 * @date 2024/09/30
 * @bug No known bugs.
 * @license OpenBSD ISC
 */
#ifdef __unix__
#define _POSIX_C_SOURCE 200809L
#include "pexec.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

error_t
pexec(char const *_cmd[], FILE **_fp0, FILE **_fp1, PEXEC_PID *_pid)
{
	int      pid = -1;
	int	 fd[2][2] = {{-1,-1},{-1,-1}};
	FILE	*fp[2] = {NULL,NULL};
	
	if (pipe(fd[0]) == -1 || pipe(fd[1]) == -1) {
		goto cleanup;
	}
	
	switch ((pid = fork())) {
	case -1:
		goto cleanup;
	case 0:
		dup2(fd[0][0], 0);
		dup2(fd[1][1], 1);
		close(fd[0][0]); close(fd[1][0]);
		close(fd[0][1]); close(fd[1][1]);
		
		execvp(_cmd[0], (char * const *)_cmd);
		fprintf(stderr, "error: %s: %s.\n", _cmd[0], strerror(errno));
		exit(1);
	default:
		close(fd[0][0]); fd[0][0] = -1;
		close(fd[1][1]); fd[1][1] = -1;
	}
	
	fp[1] = fdopen(fd[0][1], "w");
	fp[0] = fdopen(fd[1][0], "r");
	if (!fp[0] || !fp[1] || !fp[2]) {
		goto cleanup;
	}
	*_fp0 = fp[0];
	*_fp1 = fp[1];
	*_pid = pid;
	return NULL;
 cleanup:
	if (fd[0][0] != -1) close(fd[0][0]);
	if (fd[0][1] != -1) close(fd[0][1]);
	if (fd[1][0] != -1) close(fd[1][0]);
	if (fd[1][1] != -1) close(fd[1][1]);
	if (fp[0]) fclose(fp[0]);
	if (fp[1]) fclose(fp[1]);
	if (pid != -1) waitpid(pid, NULL, 0);
	return strerror(errno);
}

error_t
pwait(PEXEC_PID _pid)
{
	int status;
	if (waitpid(_pid, &status, 0) == -1) {
		return strerror(errno);
	} else if (!WIFEXITED(status)) {
		return "child process exited abnormally";
	} else if (WEXITSTATUS(status) != 0) {
		return "child process exited with non-zero status";
	} else {
		return NULL;
	}
}

error_t
pkill(PEXEC_PID _pid)
{
	if (kill(_pid, SIGKILL) == -1) {
		return strerror(errno);
	} else {
		return NULL;
	}
}

#endif
