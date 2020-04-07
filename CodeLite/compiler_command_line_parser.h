//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : compiler_command_line_parser.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include "codelite_exports.h"
#include <set>
#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/string.h>

class WXDLLIMPEXP_CL CompilerCommandLineParser
{
public:
    typedef std::set<wxString> Set_t;

protected:
    wxArrayString m_includes;
    wxArrayString m_macros;
    wxArrayString m_macrosWithPrefix;
    wxArrayString m_includesWithPrefix;
    wxArrayString m_framworks;
    wxArrayString m_sysroots; // -isysroot
    wxString m_standard;
    wxArrayString m_otherOptions;

    int m_argc;
    char** m_argv;
    wxString m_diretory;
    wxString m_pchFile;

protected:
    void AddIncludesFromFile(const wxFileName& includeFile);

public:
    CompilerCommandLineParser(const wxString& cmdline, const wxString& workingDirectory = wxEmptyString);
    virtual ~CompilerCommandLineParser();

    wxString GetCompileLine() const;
    void MakeAbsolute(const wxString& path);
    const wxString& GetStandard() const { return m_standard; }

    wxString GetStandardWithPrefix() const;

    const wxArrayString& GetSysroots() const { return m_sysroots; }
    const wxArrayString& GetOtherOptions() const { return m_otherOptions; }
    const wxArrayString& GetFramworks() const { return m_framworks; }
    const wxString& GetPchFile() const { return m_pchFile; }
    void SetArgc(int argc) { this->m_argc = argc; }
    void SetArgv(char** argv) { this->m_argv = argv; }
    void SetIncludes(const wxArrayString& includes) { this->m_includes = includes; }
    void SetMacros(const wxArrayString& macros) { this->m_macros = macros; }
    const wxArrayString& GetIncludes() const { return m_includes; }
    const wxArrayString& GetMacros() const { return m_macros; }
    char** GetArgv() { return m_argv; }

    int GetArgc() const { return m_argc; }
    const wxString& GetDiretory() const { return m_diretory; }
    const wxArrayString& GetIncludesWithPrefix() const { return m_includesWithPrefix; }
    const wxArrayString& GetMacrosWithPrefix() const { return m_macrosWithPrefix; }
};

#endif // COMMANDLINEPARSER_H
