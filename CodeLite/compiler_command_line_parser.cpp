#include "compiler_command_line_parser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wx/string.h>


//////////////////////////////////////////////////////
// Helper C functions
//////////////////////////////////////////////////////

#ifndef EOS
#define EOS '\0'
#endif

#ifdef __WXMSW__
#include <malloc.h>
#endif

#define ISBLANK(ch) ((ch) == ' ' || (ch) == '\t')
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


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

char **buildargv (const char *input, int &argc)
{
	char *arg;
	char *copybuf;
	int squote = 0;
	int dquote = 0;
	int bsquote = 0;
	int maxargc = 0;
	char **argv = NULL;
	char **nargv;

	if (input != NULL) {
		copybuf = (char *) alloca (strlen (input) + 1);
		/* Is a do{}while to always execute the loop once.  Always return an
		argv, even for null strings.  See NOTES above, test case below. */
		do {
			/* Pick off argv[argc] */
			while (ISBLANK(*input)) {
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

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

CompilerCommandLineParser::CompilerCommandLineParser(const wxString &cmdline)
{
	m_argc = 0;
	m_argv = NULL;
	
	wxString c = cmdline;
    
	// HACK
	// Since our code does not handle \ properly when its part of a directory name
	// we replace it with forward slash
	c.Replace(wxT("\\\""), wxT("@@GERESH@@"));
	c.Replace(wxT("\\"), wxT("/"));
	c.Replace(wxT("@@GERESH@@"), wxT("\\\""));
	
	// Check for makefile directory changes lines
	if(cmdline.Contains(wxT("Entering directory `"))) {
		wxString currentDir = cmdline.AfterFirst(wxT('`'));
		m_diretory = currentDir.BeforeLast(wxT('\''));

	} else {
	
		m_argv = buildargv(c.mb_str(wxConvUTF8).data(), m_argc);

		for(int i=0; i<m_argc; i++) {
			wxString opt = wxString(m_argv[i], wxConvUTF8);
			opt.Trim().Trim(false);
			
			wxString rest;
			if(opt.StartsWith(wxT("-I"), &rest)) {
				m_includes.Add(rest);
				m_includesWithPrefix.Add(opt);
			}
			
			else if(opt.StartsWith(wxT("-D"), &rest)) {
				m_macros.Add(rest);
				m_macrosWithPrefix.Add(opt);
			} 
            
            else if(opt.StartsWith(wxT("-include-path"), &rest)) {
                m_includesWithPrefix.Add(opt);
                
                rest.Trim().Trim(false);
                m_pchFile = rest;
            }
		}
	}
}

CompilerCommandLineParser::~CompilerCommandLineParser()
{
	freeargv(m_argv);
	m_argv = NULL;
	m_argc = 0;
}

wxString CompilerCommandLineParser::GetCompileLine() const
{
	wxString s;
	for(size_t i=0; i<m_includes.GetCount(); i++) {
		s << wxT("-I") << m_includes.Item(i) << wxT(" ");
	}
	
	for(size_t i=0; i<m_macros.GetCount(); i++) {
		s << wxT("-D") << m_macros.Item(i) << wxT(" ");
	}
	
	s.Trim().Trim(false);
	return s;
}
