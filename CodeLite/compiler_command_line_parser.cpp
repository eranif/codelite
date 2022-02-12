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

#include "StringUtils.h"
#include "procutils.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/string.h>

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
    c.Replace("\\\"", "@@GERESH@@");
    c.Replace("\\", "/");
    c.Replace("@@GERESH@@", "\\\"");

    // Check for makefile directory changes lines
    if(cmdline.Contains("Entering directory `")) {
        wxString currentDir = cmdline.AfterFirst('`');
        m_diretory = currentDir.BeforeLast('\'');

    } else {

        m_argv = StringUtils::BuildArgv(c, m_argc);

        for(int i = 0; i < m_argc; i++) {
            wxString opt = wxString(m_argv[i], wxConvUTF8);
            opt.Trim().Trim(false);

            wxString rest;
            if(opt.StartsWith("`") && opt.EndsWith("`")) {
                // backtick
                opt = opt.Mid(1);
                opt.RemoveLast();

                // Execute the command
                wxString result = ProcUtils::SafeExecuteCommand(opt);

                // Parse the result
                CompilerCommandLineParser cclp(result, workingDirectory);
                m_includes.insert(m_includes.end(), cclp.GetIncludes().begin(), cclp.GetIncludes().end());
                m_includesWithPrefix.insert(m_includesWithPrefix.end(), cclp.GetIncludesWithPrefix().begin(),
                                            cclp.GetIncludesWithPrefix().end());
                m_macros.insert(m_macros.end(), cclp.GetMacros().begin(), cclp.GetMacros().end());
                m_macrosWithPrefix.insert(m_macrosWithPrefix.end(), cclp.GetMacrosWithPrefix().begin(),
                                          cclp.GetMacrosWithPrefix().end());
                m_framworks.insert(m_framworks.end(), cclp.GetFramworks().begin(), cclp.GetFramworks().end());
            } else if(opt.StartsWith("@") && (opt.Contains("includes_C.rsp") || opt.Contains("includes_CXX.rsp"))) {

                // The include folders are inside the file - read the file and process its content
                wxFileName fnIncludes(workingDirectory + "/" + opt.Mid(1));
                if(fnIncludes.Exists()) {
                    AddIncludesFromFile(fnIncludes);
                }

            } else if(opt == "-isystem" && (i + 1 < m_argc)) {

                // the next arg is the include folder
                wxString include_path = m_argv[i + 1];
                include_path.Trim().Trim(false);

                m_includes.Add(include_path);
                m_includesWithPrefix.Add(wxString() << "-I" << include_path);
                ++i;

            } else if(opt.StartsWith("-I", &rest)) {
                rest.Replace("\"", wxEmptyString);
                wxFileName path(rest, wxEmptyString);
                m_includes.Add(path.GetPath());
                m_includesWithPrefix.Add("-I" + path.GetPath());

            } else if(opt.StartsWith("/I", &rest)) {
                rest.Replace("\"", wxEmptyString);
                wxFileName path(rest, wxEmptyString);
                m_includes.Add(path.GetPath());
                m_includesWithPrefix.Add("/I" + path.GetPath());

            }

            else if(opt.StartsWith("-D", &rest) || opt.StartsWith("/D", &rest)) {
                m_macros.Add(rest);
                m_macrosWithPrefix.Add(opt);
            }

            else if(opt.StartsWith("-include-path ", &rest)) {
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

            else if(opt.StartsWith("/FI", &rest)) {
                rest.Trim().Trim(false);
                m_pchFile = rest;
            }

            // Support for Apple's Framework include paths
            else if(opt.StartsWith("-F", &rest)) {

                m_includesWithPrefix.Add(opt);
                rest.Trim().Trim(false);
                m_framworks.Add(rest);

            }

            // std
            else if(opt.StartsWith("-std=", &rest) || opt.StartsWith("/std:", &rest)) {
                rest.Trim().Trim(false);

                if(!rest.IsEmpty()) {
                    m_standard = rest;
                }
                // keep the std as an option as well
                m_otherOptions.Add(opt);
            } else {
                m_otherOptions.Add(opt);
            }
        }
    }
}

CompilerCommandLineParser::~CompilerCommandLineParser()
{
    StringUtils::FreeArgv(m_argv, m_argc);
    m_argv = NULL;
    m_argc = 0;
}

wxString CompilerCommandLineParser::GetCompileLine() const
{
    wxString s;
    for(size_t i = 0; i < m_includes.GetCount(); i++) {
        s << "-I" << m_includes.Item(i) << " ";
    }

    for(size_t i = 0; i < m_macros.GetCount(); i++) {
        s << "-D" << m_macros.Item(i) << " ";
    }

    for(size_t i = 0; i < m_sysroots.size(); ++i) {
        s << "-isysroot " << m_sysroots.Item(i) << " ";
    }
    s.Trim().Trim(false);
    return s;
}

wxString CompilerCommandLineParser::GetStandardWithPrefix() const
{
    if(m_standard.IsEmpty()) {
        return "";
    }
    return "-std=" + m_standard;
}

void CompilerCommandLineParser::MakeAbsolute(const wxString& path)
{
    wxArrayString incls;
    incls.reserve(m_includes.size());

    for(size_t i = 0; i < m_includes.GetCount(); ++i) {
        wxFileName fn(m_includes.Item(i), "");
        fn.MakeAbsolute(path);
        incls.Add(fn.GetPath());
    }
    m_includes.swap(incls);

    m_includesWithPrefix.Clear();
    for(size_t i = 0; i < m_framworks.GetCount(); ++i) {
        m_includesWithPrefix.Add("-F" + m_framworks.Item(i));
    }

    for(size_t i = 0; i < m_includes.GetCount(); ++i) {
        m_includesWithPrefix.Add("-I" + m_includes.Item(i));
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
