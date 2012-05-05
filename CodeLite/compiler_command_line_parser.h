#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <wx/string.h>
#include <wx/arrstr.h>
#include "codelite_exports.h"


class WXDLLIMPEXP_CL CommandLineParser
{
	wxArrayString m_includes;
	wxArrayString m_macros;
	int m_argc;
	char **m_argv;

public:
	CommandLineParser(const wxString &cmdline);
	virtual ~CommandLineParser();
	
	wxString GetCompileLine() const;
	void SetArgc(int argc) {
		this->m_argc = argc;
	}
	void SetArgv(char** argv) {
		this->m_argv = argv;
	}
	void SetIncludes(const wxArrayString& includes) {
		this->m_includes = includes;
	}
	void SetMacros(const wxArrayString& macros) {
		this->m_macros = macros;
	}
	const wxArrayString& GetIncludes() const {
		return m_includes;
	}
	const wxArrayString& GetMacros() const {
		return m_macros;
	}
	char** GetArgv() {
		return m_argv;
	}

	int GetArgc() const {
		return m_argc;
	}
};

#endif // COMMANDLINEPARSER_H
