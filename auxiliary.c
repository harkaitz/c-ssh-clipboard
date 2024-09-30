#include "auxiliary.h"
#include "pexec.h"
#ifdef __unix__
#  define ENV_HOME "HOME"
#endif
#ifdef _WIN32
#  define ENV_HOME "USERPROFILE"
#endif

error_t
fopen_home(FILE **_fp, char const _f[], char const _m[])
{
	char	*home = getenv(ENV_HOME);
	char	 path[((home)?strlen(home):0) + strlen(_f) + 10];
	FILE	*fp;
	
	if (_f[0] == '~') {
		if (!home) {
			return "Can't find the home directory";
			return NULL;
		}
		sprintf(path, "%s/%s", home, _f+1);
	} else {
		strcpy(path, _f);
	}
	fp = fopen(path, _m);
	if (!fp) {
		return strerror(errno);
	}
	*_fp = fp;
	return NULL;;
}

int /* 0: success, -1: failure */
popen_ssh(FILE **_fp0, FILE **_fp1, PEXEC_PID *_pid)
{
	char	*v;
	FILE	*fp = NULL;
	error_t	 err;
	char	 machine[1024] = {0};
	if ((v = getenv("SSH_CLIPBOARD"))) {
		strncpy(machine, v, sizeof(machine)-1);
	}
	if (!machine[0] && (!fopen_home(&fp, "~/.ssh/clipboard", "r"))) {
		fgets(machine, sizeof(machine)-1, fp);
		if ((v = strchr(machine, '\r'))) *v = '\0';
		if ((v = strchr(machine, '\n'))) *v = '\0';
		fclose(fp);
	}
	if (!machine[0]) {
		fprintf(stderr, "ssh-clipboard: error: Specify the machine in $SSH_CLIPBOARD or ~/.ssh/clipboard.\n");
		return -1;
	}
	char const *cmd[] = {"ssh", "-sT", machine, "ssh-clipboard", NULL};
	err = pexec(cmd, _fp0, _fp1, _pid);
	if (err != NULL) {
		fprintf(stderr, "ssh-clipboard: error: Executing ssh: %s\n", err);
		return -1;
	}
	return 0;
}

int /* 0: success, -1: failure */
read_prompt(PEXEC_PID pid, FILE *_fp0, FILE *_fp1, char const *_step)
{
	char	*v;
	char	 buf[1024];
	if (_fp1) {
		fflush(_fp1);
		fflush(stdout);
	}
	if (_fp0) {
		if (!fgets(buf, sizeof(buf), _fp0)) {
			pwait(pid);
			return -1;
		}
		if ((v = strchr(buf, '\r'))) *v = '\0';
		if ((v = strchr(buf, '\n'))) *v = '\0';
		if (strcmp(buf, "[ssh-clipboard]")) {
			fprintf(stderr, "ssh-clipboard: error: %s: %s\n", _step, buf);
			pkill(pid);
			pwait(pid);
			return -1;
		}
	}
	return 0;
}
