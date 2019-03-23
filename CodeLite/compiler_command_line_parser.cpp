//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : compiler_command_line_parser.cpp
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

#include "compiler_command_line_parser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/ffile.h>

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
#define INITIAL_MAXARGC 8 /* Number of args + NULL in initial argv */

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

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

char** buildargv(const char* input, int& argc)
{
    char* arg;
    char* copybuf;
    int squote = 0;
    int dquote = 0;
    int bsquote = 0;
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

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

CompilerCommandLineParser::CompilerCommandLineParser(const wxString& cmdline, const wxString& workingDirectory)
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

        for(int i = 0; i < m_argc; i++) {
            wxString opt = wxString(m_argv[i], wxConvUTF8);
            opt.Trim().Trim(false);

            wxString rest;

            if(opt.StartsWith("@") && (opt.Contains("includes_C.rsp") || opt.Contains("includes_CXX.rsp"))) {

                // The include folders are inside the file - read the file and process its content
                wxFileName fnIncludes(workingDirectory + "/" + opt.Mid(1));
                if(fnIncludes.Exists()) { AddIncludesFromFile(fnIncludes); }

            } else if(opt == "-isystem" && (i + 1 < m_argc)) {

                // the next arg is the include folder
                wxString include_path = m_argv[i + 1];
                include_path.Trim().Trim(false);

                m_includes.Add(include_path);
                m_includesWithPrefix.Add(wxString() << "-I" << include_path);
                ++i;

            } else if(opt.StartsWith(wxT("-I"), &rest)) {
                m_includes.Add(rest);
                m_includesWithPrefix.Add(opt);
            }

            else if(opt.StartsWith(wxT("-D"), &rest)) {
                m_macros.Add(rest);
                m_macrosWithPrefix.Add(opt);
            }

            else if(opt.StartsWith(wxT("-include-path "), &rest)) {
                m_includesWithPrefix.Add(rest);
                rest.Trim().Trim(false);
                m_pchFile = rest;
            }

            else if((opt == "-include") && (i + 1 < m_argc)) {
                wxString include_path = m_argv[i + 1];
                include_path.Trim().Trim(false);
                m_pchFile = include_path;
                ++i;
            } else if((opt == "-isysroot") && (i + 1 < m_argc)) {
                wxString include_path = m_argv[i + 1];
                include_path.Trim().Trim(false);
                m_sysroots.Add(include_path);
                ++i;
            }

            // Support for Apple's Framework include paths
            else if(opt.StartsWith(wxT("-F"), &rest)) {

                m_includesWithPrefix.Add(opt);
                rest.Trim().Trim(false);
                m_framworks.Add(rest);

            }

            // Support for Apple's Framework include paths
            else if(opt.StartsWith(wxT("-std"), &rest)) {
                wxString stds = rest.AfterFirst(wxT('='));
                stds.Trim().Trim(false);

                if(stds.IsEmpty() == false) { m_standard = stds; }
            } else {
                m_otherOptions.Add(opt);
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
    for(size_t i = 0; i < m_includes.GetCount(); i++) {
        s << wxT("-I") << m_includes.Item(i) << wxT(" ");
    }

    for(size_t i = 0; i < m_macros.GetCount(); i++) {
        s << wxT("-D") << m_macros.Item(i) << wxT(" ");
    }

    for(size_t i = 0; i < m_sysroots.size(); ++i) {
        s << "-isysroot " << m_sysroots.Item(i) << " ";
    }
    s.Trim().Trim(false);
    return s;
}

wxString CompilerCommandLineParser::GetStandardWithPrefix() const
{
    if(m_standard.IsEmpty()) return wxT("");

    return wxT("-std=") + m_standard;
}

void CompilerCommandLineParser::MakeAbsolute(const wxString& path)
{
    wxArrayString incls;
    incls.reserve(m_includes.size());

    for(size_t i = 0; i < m_includes.GetCount(); ++i) {
        wxFileName fn(m_includes.Item(i), wxT(""));
        fn.MakeAbsolute(path);
        incls.Add(fn.GetPath());
    }
    m_includes.swap(incls);

    m_includesWithPrefix.Clear();
    for(size_t i = 0; i < m_framworks.GetCount(); ++i) {
        m_includesWithPrefix.Add(wxT("-F") + m_framworks.Item(i));
    }

    for(size_t i = 0; i < m_includes.GetCount(); ++i) {
        m_includesWithPrefix.Add(wxT("-I") + m_includes.Item(i));
    }
}

void CompilerCommandLineParser::AddIncludesFromFile(const wxFileName& includeFile)
{
    wxFFile fp(includeFile.GetFullPath(), "rb");
    if(fp.IsOpened()) {
        wxString content;
        fp.ReadAll(&content);
        content.Replace("\n", " ");
        CompilerCommandLineParser cclp(content);
        m_includes.insert(m_includes.end(), cclp.GetIncludes().begin(), cclp.GetIncludes().end());
        m_includesWithPrefix.insert(m_includesWithPrefix.end(), cclp.GetIncludesWithPrefix().begin(),
                                    cclp.GetIncludesWithPrefix().end());
        fp.Close();
    }
}
