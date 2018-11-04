#if defined(__WXGTK__) || defined(__WXOSX__)
#include <pty.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utmp.h>
#include <wx/string.h>

#ifndef EOS
#define EOS '\0'
#endif

#define INITIAL_MAXARGC 8 /* Number of args + NULL in initial argv */

static char** argv = NULL;
static int argc = 0;

static void freeargv(char** vector)
{
    register char** scan;

    if(vector != NULL) {
        for(scan = vector; *scan != NULL; scan++) {
            free(*scan);
        }
        free(vector);
    }
}

char** dupargv(char** argv)
{
    int argc;
    char** copy;

    if(argv == NULL) return NULL;

    /* the vector */
    for(argc = 0; argv[argc] != NULL; argc++)
        ;
    copy = (char**)malloc((argc + 1) * sizeof(char*));
    if(copy == NULL) return NULL;

    /* the strings */
    for(argc = 0; argv[argc] != NULL; argc++) {
        int len = strlen(argv[argc]);
        copy[argc] = (char*)malloc(sizeof(char*) * (len + 1));
        if(copy[argc] == NULL) {
            freeargv(copy);
            return NULL;
        }
        strcpy(copy[argc], argv[argc]);
    }
    copy[argc] = NULL;
    return copy;
}

#ifndef ISBLANK
#define ISBLANK(c) ((c == ' ') || (c == '\t'))
#endif

char** buildargv(const char* input)
{
    char* arg;
    char* copybuf;
    int squote = 0;
    int dquote = 0;
    int bsquote = 0;
    int argc = 0;
    int maxargc = 0;
    char** argv = NULL;
    char** nargv;

    if(input != NULL) {
        copybuf = (char*)alloca(strlen(input) + 1);
        /* Is a do{}while to always execute the loop once.  Always return an
        argv, even for null strings.  See NOTES above, test case below. */
        do {
            /* Pick off argv[argc] */
            while(ISBLANK(*input)) {
                input++;
            }
            if((maxargc == 0) || (argc >= (maxargc - 1))) {
                /* argv needs initialization, or expansion */
                if(argv == NULL) {
                    maxargc = INITIAL_MAXARGC;
                    nargv = (char**)malloc(maxargc * sizeof(char*));
                } else {
                    maxargc *= 2;
                    nargv = (char**)realloc(argv, maxargc * sizeof(char*));
                }
                if(nargv == NULL) {
                    if(argv != NULL) {
                        freeargv(argv);
                        argv = NULL;
                    }
                    break;
                }
                argv = nargv;
                argv[argc] = NULL;
            }
            /* Begin scanning arg */
            arg = copybuf;
            while(*input != EOS) {
                if(ISBLANK(*input) && !squote && !dquote && !bsquote) {
                    break;
                } else {
                    if(bsquote) {
                        bsquote = 0;
                        *arg++ = *input;
                    } else if(*input == '\\') {
                        bsquote = 1;
                    } else if(squote) {
                        if(*input == '\'') {
                            squote = 0;
                        } else {
                            *arg++ = *input;
                        }
                    } else if(dquote) {
                        if(*input == '"') {
                            dquote = 0;
                        } else {
                            *arg++ = *input;
                        }
                    } else {
                        if(*input == '\'') {
                            squote = 1;
                        } else if(*input == '"') {
                            dquote = 1;
                        } else {
                            *arg++ = *input;
                        }
                    }
                    input++;
                }
            }
            *arg = EOS;
            argv[argc] = strdup(copybuf);
            if(argv[argc] == NULL) {
                freeargv(argv);
                argv = NULL;
                break;
            }
            argc++;
            argv[argc] = NULL;

            while(ISBLANK(*input)) {
                input++;
            }
        } while(*input != EOS);
    }
    return (argv);
}

//-----------------------------------------------------

char** make_argv(const std::string& cmd, int& _argc)
{
    if(argc) {
        freeargv(argv);
        _argc = 0;
    }

    argv = buildargv(cmd.c_str());
    argc = 0;

    for(char** targs = argv; *targs != NULL; targs++) {
        argc++;
    }
    _argc = argc;
    return argv;
}

int StartProcess(const wxString& nodejs, const wxString& scriptPath, int& pid)
{
    pid = 0;
    int master, slave;
    openpty(&master, &slave, NULL, NULL, NULL);

    int _argc = 0;
    wxString command;
    command << nodejs << " inspect " << scriptPath;

    pid = fork();
    if(pid == 0) {
        login_tty(slave);
        close(master); // close the un-needed master end

        char** _argv = make_argv(command.mb_str(wxConvUTF8).data(), _argc);
        execvp(_argv[0], _argv);

        // Nothing below this line should be executed by child process. If so,
        // it means that the execl function wasn't successfull, so lets exit:
        exit(1);
    }

    // Parent code
    // remove the echo
    close(slave);

    struct termios tios;
    tcgetattr(master, &tios);
    tios.c_lflag &= ~(ECHO | ECHONL);
    tcsetattr(master, TCSAFLUSH, &tios);
    return master;
}
#endif // USE_FORK