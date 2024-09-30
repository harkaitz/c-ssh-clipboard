#include "auxiliary.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

char const HELP[] =
    "Usage: ssh-clipboard "                                                 "\n"
    ""                                                                      "\n"
    "Provide a simple clipboard in \"~/.clipboard.{data,id}\". It supports" "\n"
    "the following commands:"                                               "\n"
    ""                                                                      "\n"
    "  write     Read until EOF and save in ~/.clipboard.data"              "\n"
    "  setid ID  Save an ID in ~/.clipboard.id"                             "\n"
    "  chkid ID  Check that the ID in ~/.clipboard.id matches."             "\n"
    "  read      Print the contents of ~/.clipboard.data, will close."      "\n"
    "  clear     Remove ~/.clipboard.data and ~/.clipboard.id at quit."     "\n"
    ""                                                                      "\n"
    "Copyright (c) 2024  --  Harkaitz Agirre  -- OpenBSD ISC license"       "\n"
    ;

static error_t cmd_write(void);
static error_t cmd_read(void);
static error_t cmd_setid(char *);
static error_t cmd_chkid(char *);

char buf[4096];

int
main(int _argc, char *_argv[])
{
	char	 line[512] = {0};
	char	*cmd, *arg;
	error_t	 err = NULL;

	if (_argc > 1 && (!strcmp(_argv[1], "-h") || !strcmp(_argv[1], "--help"))) {
		fputs(HELP, stdout);
		return 0;
	}

	for(;;) {

		/* Read a command.*/
		fputs("[ssh-clipboard]\n", stdout);
		fflush(stdout);
		if (!fgets(line, sizeof(line)-1, stdin))
			break;
		cmd = strtok(line, " \t\n\r");
		if (!cmd)
			continue;
		arg = strtok(NULL, "\t\n\r");

		/* Execute the commands. */
		if (!strcmp(cmd, "write")) {
			err = cmd_write();
		} else if (!strcmp(cmd, "setid")) {
			err = cmd_setid(arg);
		} else if (!strcmp(cmd, "chkid")) {
			err = cmd_chkid(arg);
		} else if (!strcmp(cmd, "read")) {
			err = cmd_read();
			break;
		} else {
			err = "Unsupported command";
		}
		

		/* When failure quit. */
		if (err) {
			fputs(err, stdout);
			break;
		}
	}

	fflush(stdout);
	return (err)?1:0;
}

static error_t
cmd_write(void)
{
	FILE	*fp = NULL;
	size_t	 n;
	error_t	 err;

	/* Open output file. */
	if ((err = fopen_home(&fp, "~/.clipboard.data", "wb")))
		return err;

	/* Write from standard input to the file. */
	while ((n = fread(buf, 1, sizeof(buf), stdin)) > 0) {
		if (fwrite(buf, 1, n, fp) != n) {
			fclose(fp);
			return "Write failed";
		}
		if (n < sizeof(buf)) {
			break;
		}
	}

	/* Close the file. */
	fclose(fp);
	return 0;
}

static error_t
cmd_read(void)
{
	FILE	*fp = NULL;
	int	 n;
	error_t	 err;

	/* Open input file. */
	if ((err = fopen_home(&fp, "~/.clipboard.data", "rb")))
		return err;
	fputs("[ssh-clipboard]\n", stdout);

	/* Read from the file to standard output. */
	while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
		if (fwrite(buf, 1, n, stdout) != n) {
			return NULL;
		}
	}

	/* Close the file. */
	fclose(fp);
	return 0;
}

static error_t
cmd_setid(char *_id)
{
	FILE	*fp = NULL;
	error_t	 err;
	if ((err = fopen_home(&fp, "~/.clipboard.id", "w")))
		return err;
	fprintf(fp, "%s\n", (_id)?_id:"default");
	fclose(fp);
	return 0;
}

static error_t
cmd_chkid(char *_id)
{
	FILE	*fp = NULL;
	char	 id[256] = {0};
	char	*nl;
	error_t	 err;
	if ((err = fopen_home(&fp, "~/.clipboard.id", "r")))
		return err;
	fgets(id, sizeof(id)-1, fp);
	if ((nl = strchr(id, '\n')))
		*nl = 0;
	if (strcmp(id, (_id)?_id:"default")) {
		fclose(fp);
		return "ID mismatch";
	}
	fclose(fp);
	return NULL;
}
