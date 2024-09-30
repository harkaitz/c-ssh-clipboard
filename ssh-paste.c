#include "auxiliary.h"

char const HELP[] =
    "Usage: ssh-paste [ID] > DATA"   "\n";

char buf[4096];

int
main(int _argc, char *_argv[])
{
	FILE		*fp[2] = {NULL, NULL};
	PEXEC_PID	 pid;
	int		 e;

	if (_argc > 1 && (!strcmp(_argv[1], "-h") || !strcmp(_argv[1], "--help"))) {
		fputs(HELP, stdout);
		return 0;
	}

	e = popen_ssh(&fp[0], &fp[1], &pid);
	if (e == -1/*err*/) { return 1; }

	e = read_prompt(pid, fp[0], fp[1], "1");
	if (e == -1/*err*/) { return 1; }

	if (_argc == 2) {
		fprintf(fp[1], "chkid %s\n", _argv[1]);
		e = read_prompt(pid, fp[0], fp[1], "1*");
		if (e == -1/*err*/) { return 1; }
	}

	fprintf(fp[1], "read\n");
	e = read_prompt(pid, fp[0], fp[1], "2");
	if (e == -1/*err*/) { return 1; }

	while ((e = fread(buf, 1, sizeof(buf), fp[0])) > 0) {
		if (fwrite(buf, 1, e, stdout) != e) {
			fprintf(stderr, "ssh-paste: error: fwrite: %s.\n", strerror(errno));
			return 1;
		}
		if (e < sizeof(buf)) {
			break;
		}
	}
	fclose(fp[0]);
	
	e = read_prompt(pid, NULL, fp[1], "3");
	if (e == -1/*err*/) { return 1; }
	fclose(fp[1]);
	
	return 0;
}
