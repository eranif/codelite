#ifndef _WIN32

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/stat.h>

static char  **argv = NULL;
static int    argc = 0;

// ----------------------------------------------
#define ISBLANK(ch) ((ch) == ' ' || (ch) == '\t')
#define BUFF_SIZE  1024*64

/*  Routines imported from standard C runtime libraries. */

#ifdef __STDC__

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#else	/* !__STDC__ */

#if !defined _WIN32 || defined __GNUC__
extern char *memcpy ();		/* Copy memory region */
extern int strlen ();		/* Count length of string */
extern char *malloc ();		/* Standard memory allocater */
extern char *realloc ();	/* Standard memory reallocator */
extern void free ();		/* Free malloc'd memory */
extern char *strdup ();		/* Duplicate a string */
#endif

#endif	/* __STDC__ */


#ifndef NULL
#define NULL 0
#endif

#ifndef EOS
#define EOS '\0'
#endif

#define INITIAL_MAXARGC 8	/* Number of args + NULL in initial argv */

static void freeargv (char **vector)
{
	register char **scan;

	if (vector != NULL) {
		for (scan = vector; *scan != NULL; scan++) {
			free (*scan);
		}
		free (vector);
	}
}

char **
dupargv (char **argv)
{
	int argc;
	char **copy;

	if (argv == NULL)
		return NULL;

	/* the vector */
	for (argc = 0; argv[argc] != NULL; argc++);
	copy = (char **) malloc ((argc + 1) * sizeof (char *));
	if (copy == NULL)
		return NULL;

	/* the strings */
	for (argc = 0; argv[argc] != NULL; argc++) {
		int len = strlen (argv[argc]);
		copy[argc] = (char*)malloc (sizeof (char *) * (len + 1));
		if (copy[argc] == NULL) {
			freeargv (copy);
			return NULL;
		}
		strcpy (copy[argc], argv[argc]);
	}
	copy[argc] = NULL;
	return copy;
}

char **buildargv (const char *input)
{
	char *arg;
	char *copybuf;
	int squote = 0;
	int dquote = 0;
	int bsquote = 0;
	int argc = 0;
	int maxargc = 0;
	char **argv = NULL;
	char **nargv;

	if (input != NULL) {
		copybuf = (char *) alloca (strlen (input) + 1);
		/* Is a do{}while to always execute the loop once.  Always return an
		argv, even for null strings.  See NOTES above, test case below. */
		do {
			/* Pick off argv[argc] */
			while (ISBLANK (*input)) {
				input++;
			}
			if ((maxargc == 0) || (argc >= (maxargc - 1))) {
				/* argv needs initialization, or expansion */
				if (argv == NULL) {
					maxargc = INITIAL_MAXARGC;
					nargv = (char **) malloc (maxargc * sizeof (char *));
				} else {
					maxargc *= 2;
					nargv = (char **) realloc (argv, maxargc * sizeof (char *));
				}
				if (nargv == NULL) {
					if (argv != NULL) {
						freeargv (argv);
						argv = NULL;
					}
					break;
				}
				argv = nargv;
				argv[argc] = NULL;
			}
			/* Begin scanning arg */
			arg = copybuf;
			while (*input != EOS) {
				if (ISBLANK (*input) && !squote && !dquote && !bsquote) {
					break;
				} else {
					if (bsquote) {
						bsquote = 0;
						*arg++ = *input;
					} else if (*input == '\\') {
						bsquote = 1;
					} else if (squote) {
						if (*input == '\'') {
							squote = 0;
						} else {
							*arg++ = *input;
						}
					} else if (dquote) {
						if (*input == '"') {
							dquote = 0;
						} else {
							*arg++ = *input;
						}
					} else {
						if (*input == '\'') {
							squote = 1;
						} else if (*input == '"') {
							dquote = 1;
						} else {
							*arg++ = *input;
						}
					}
					input++;
				}
			}
			*arg = EOS;
			argv[argc] = strdup (copybuf);
			if (argv[argc] == NULL) {
				freeargv (argv);
				argv = NULL;
				break;
			}
			argc++;
			argv[argc] = NULL;

			while (ISBLANK (*input)) {
				input++;
			}
		} while (*input != EOS);
	}
	return (argv);
}

//-----------------------------------------------------

static void make_argv(const std::string& cmd)
{
	if(argc) {
		freeargv(argv);
		argc=0;
	}

	argv = buildargv(cmd.c_str());
	argc=0;

	for (char **targs = argv; *targs != NULL; targs++) {
		argc++;
	}
}

#define BUFF_STATE_NORMAL 0
#define BUFF_STATE_IN_ESC 1

static void RemoveTerminalColoring(char *buffer)
{
	char *saved_buff = buffer;
	char tmpbuf[BUFF_SIZE+1];
	memset(tmpbuf, 0, sizeof(tmpbuf));

	short state = BUFF_STATE_NORMAL;
	size_t i(0);

	while(*buffer != 0) {
		switch (state) {
		case BUFF_STATE_NORMAL:
			if(*buffer == 0x1B) { // found ESC char
				state = BUFF_STATE_IN_ESC;

			} else {
				tmpbuf[i] = *buffer;
				i++;
			}
			break;
		case BUFF_STATE_IN_ESC:
			if(*buffer == 'm') { // end of color sequence
				state = BUFF_STATE_NORMAL;
			}
			break;
		}
		buffer++;
	}
	memset(saved_buff, 0, BUFF_SIZE);
	memcpy(saved_buff, tmpbuf, strlen(tmpbuf));
}

int ExecuteProcessUNIX(const std::string& commandline)
{
	make_argv(commandline);
	if ( argc == 0 ) {
		return -1;
	}

	int rc = fork();
	if ( rc == 0 ) {
		// execute the process
		execvp(argv[0], argv);
		exit(0);

	} else if ( rc < 0 ) {
		return -1;

	} else {
		// Parent
		freeargv(argv);
		argc = 0;
		int status(0);

		// wait for the process to terminate
		waitpid(rc, &status, 0);

		return WEXITSTATUS(status);
	}
}

void WriteContent( const std::string& logfile, const std::string& filename, const std::string& flags )
{
	// Open the file
	int fd = ::open(logfile.c_str(), O_CREAT|O_APPEND, 0660);
	::chmod(logfile.c_str(), 0660);

	// Lock it
	if ( fd < 0 )
		return;
		
	if (::flock(fd, LOCK_EX) < 0) {
		return;
	}
	
	std::string line = filename + "|" + flags + "\n";
	
	// Write the content
	::write(fd, line.c_str(), line.length());
	
	// Release the lock
	::flock(fd, LOCK_UN);
	
	// close the fd
	::close(fd);
}

#endif
