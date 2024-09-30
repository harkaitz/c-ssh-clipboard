/**
 * @file pexec.c
 * @brief This file implements "pexec.h" for Windows machines.
 *
 * @date 2020/09/30
 * @bug No known bugs.
 * @license OpenBSD ISC
 */
#ifdef _WIN32
#include "pexec.h"
#include <windows.h>
#include <io.h>
#include <fcntl.h>

error_t
pexec(char const *_cmd[], FILE **_fp0, FILE **_fp1, PEXEC_PID *_pid)
{
	SECURITY_ATTRIBUTES sa;
	PEXEC_PID pid = INVALID_HANDLE_VALUE;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE; // Allow handle inheritance
	sa.lpSecurityDescriptor = NULL;
	HANDLE	 h[2][2] = {
		{INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE},
		{INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE}
	};
	FILE	*fp[2] = {NULL,NULL};
	char	*errm = NULL;
	
	if (!CreatePipe(&h[0][0], &h[0][1], &sa, 0) ||
	    !CreatePipe(&h[1][0], &h[1][1], &sa, 0)) {
		errm = "CreatePipe";
		goto cleanup;
	}
	SetHandleInformation(h[0][1], HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(h[1][0], HANDLE_FLAG_INHERIT, 0);
	
	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi;
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = h[0][0];
	si.hStdOutput = h[1][1];
	
	char cmd[1024] = {0};
	for (int i = 0; _cmd[i]; i++) {
		strcat(cmd, _cmd[i]);
		strcat(cmd, " ");
	}
	if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
		errm = "CreateProcess";
		goto cleanup;
	}
	pid = pi.hProcess;
	
	CloseHandle(h[0][0]); h[0][0] = INVALID_HANDLE_VALUE;
	CloseHandle(h[1][1]); h[1][1] = INVALID_HANDLE_VALUE;
	
	fp[1] = _fdopen(_open_osfhandle((intptr_t)h[0][1], _O_WRONLY), "wb");
	fp[0] = _fdopen(_open_osfhandle((intptr_t)h[1][0], _O_RDONLY), "rb");
	if (!fp[0] || !fp[1]) {
		errm = "fdopen";
		goto cleanup;
	}
	setvbuf(fp[0], NULL, _IONBF, 0);
	setvbuf(fp[1], NULL, _IONBF, 0);
	*_fp0 = fp[0];
	*_fp1 = fp[1];
	*_pid = pid;
	return 0;
 cleanup:
	if (h[0][0] != INVALID_HANDLE_VALUE) CloseHandle(h[0][0]);
	if (h[0][1] != INVALID_HANDLE_VALUE) CloseHandle(h[0][1]);
	if (h[1][0] != INVALID_HANDLE_VALUE) CloseHandle(h[1][0]);
	if (h[1][1] != INVALID_HANDLE_VALUE) CloseHandle(h[1][1]);
	if (fp[0]) fclose(fp[0]);
	if (fp[1]) fclose(fp[1]);
	return errm;
}

error_t
pwait(PEXEC_PID _pid)
{
	DWORD status;
	if (WaitForSingleObject(_pid, INFINITE) == WAIT_FAILED) {
		return "WaitForSingleObject";
	}
	if (!GetExitCodeProcess(_pid, &status)) {
		return "GetExitCodeProcess";
	}
	CloseHandle(_pid);
	if (status != 0) {
		return "Child process failed";
	}
	return NULL;
}

error_t
pkill(PEXEC_PID _pid)
{
	if (!TerminateProcess(_pid, 0)) {
		return "TerminateProcess";
	}
	return NULL;
}

#endif
