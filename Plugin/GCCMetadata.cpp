//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : includepathlocator.cpp
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

#include "GCCMetadata.hpp"

#include "CxxPreProcessor.h"
#include "clTempFile.hpp"
#include "environmentconfig.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "procutils.h"

#include <wx/dir.h>
#include <wx/fileconf.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>

// we keep a cache entries to avoid running processes
// since thread_local is broken under windows, we use
// real locks here (at least it is for MinGW 7.3)
static std::unordered_map<wxString, GCCMetadata> s_cache;
static wxCriticalSection s_cs;

void GCCMetadata::GetMetadataFromCache(const wxString& tool, const wxString& rootDir, bool is_cygwin, GCCMetadata* md)
{
    wxCriticalSectionLocker locker(s_cs);
    if(s_cache.count(tool) == 0) {
        GCCMetadata tmp(md->m_basename);
        tmp.DoLoad(tool, rootDir, is_cygwin);
        s_cache.insert({ tool, tmp });
    }

    const auto& cachedEntry = s_cache[tool];
    *md = cachedEntry;
}

GCCMetadata::GCCMetadata(const wxString& basename)
    : m_basename(basename)
{
}

GCCMetadata::~GCCMetadata() {}

void GCCMetadata::Load(const wxString& tool, const wxString& rootDir, bool is_cygwin)
{
    // find an entry in the cache
    // if no such entry exist, update the cache
    GetMetadataFromCache(tool, rootDir, is_cygwin, this);
}

void GCCMetadata::DoLoad(const wxString& tool, const wxString& rootDir, bool is_cygwin)
{
    m_searchPaths.clear();
    m_target.clear();

    // Common compiler paths - should be placed at top of the include path!
    wxString command;

    // GCC prints parts of its output to stdout and some to stderr
    // redirect all output to stdout
    wxString working_directory;
    clTempFile tmpfile;
    tmpfile.Write(wxEmptyString);

    wxFileName cxx(tool);

    clEnvList_t envlist;
    wxString pathenv;
    ::wxGetEnv("PATH", &pathenv);
    pathenv.Prepend(cxx.GetPath() + clPATH_SEPARATOR);
    envlist.push_back({ "PATH", pathenv });

#ifdef __WXMSW__
    if(::clIsCygwinEnvironment()) {
        command << cxx.GetFullName() << " -v -x c++ /dev/null -fsyntax-only";
    } else {

        // execute the command from the tool directory
        working_directory = cxx.GetPath();
        ::WrapWithQuotes(working_directory);
        command << cxx.GetFullName() << " -xc++ -E -v " << tmpfile.GetFullPath(true) << " -fsyntax-only";
    }
#else
    command << cxx.GetFullName() << " -v -x c++ /dev/null -fsyntax-only";
#endif

    wxString outputStr = RunCommand(command, working_directory, &envlist);

    wxArrayString outputArr = wxStringTokenize(outputStr, wxT("\n\r"), wxTOKEN_STRTOK);
    // Analyze the output
    bool collect(false);
    for(wxString& line : outputArr) {
        line.Trim().Trim(false);

        // search the scan starting point
        if(line.Contains(wxT("#include <...> search starts here:"))) {
            collect = true;
            continue;
        }

        if(line.Contains(wxT("End of search list."))) {
            break;
        }

        if(collect) {
            line.Replace("(framework directory)", wxEmptyString);
#ifdef __WXMSW__
            if(is_cygwin && line.StartsWith("/usr/lib")) {
                line.Replace("/usr/lib", "/lib");
            }

            if(line.StartsWith("/")) {
                // probably MSYS2 or Cygwin
                // replace the "/" with the root folder
                line.Prepend(rootDir);
            }
#endif
            // on Mac, (framework directory) appears also,
            // but it is harmless to use it under all OSs
            wxFileName includePath(line, wxEmptyString);
            includePath.Normalize();
            m_searchPaths.Add(includePath.GetPath());
        }
    }

    // get the target
    m_target = RunCommand(cxx.GetFullName() + " -dumpmachine", wxFileName(tool).GetPath(), &envlist);
    wxString versionString = RunCommand(cxx.GetFullName() + " -dumpversion", wxFileName(tool).GetPath(), &envlist);
    m_name << m_basename << "-" << versionString;

    // load macros
    clTempFile macrosFile;
    macrosFile.Write(wxEmptyString);

    wxString macros_command;
    macros_command << cxx.GetFullName() << " -dM -E -xc++ " << macrosFile.GetFullPath(true);

    clDEBUG() << "Loading built-in macros for compiler '" << GetName() << "' :" << macros_command << clEndl;
    wxString macros = RunCommand(macros_command, working_directory, &envlist);
    macrosFile.Write(macros);

    clDEBUG() << "Compiler builtin macros are written into:" << macrosFile.GetFullPath() << endl;

    // we got our macro files
    CxxPreProcessor pp;
    wxFileName cmpMacrosFile(macrosFile.GetFullPath());
    pp.Parse(cmpMacrosFile, kLexerOpt_CollectMacroValueNumbers);
    m_macros = pp.GetDefinitions();

    LOG_IF_TRACE { clDEBUG1() << "GCC Metadata loaded for tool" << tool << endl; }
    LOG_IF_TRACE { clDEBUG1() << "Name:" << GetName() << endl; }
    LOG_IF_TRACE { clDEBUG1() << "Search paths:" << GetSearchPaths() << endl; }
    LOG_IF_TRACE { clDEBUG1() << "Macros:" << GetMacros() << endl; }
    LOG_IF_TRACE { clDEBUG1() << "Target:" << GetTarget() << endl; }
}

wxString GCCMetadata::RunCommand(const wxString& command, const wxString& working_directory, clEnvList_t* env)
{
    clDEBUG() << "Running command:" << command << endl;
    wxString outputStr;
    IProcess::Ptr_t proc(::CreateSyncProcess(command, IProcessCreateDefault, working_directory, env));
    if(proc) {
        proc->WaitForTerminate(outputStr);
    }
    LOG_IF_TRACE { clDEBUG1() << "Output is:" << outputStr << endl; }
    outputStr.Trim().Trim(false);
    return outputStr;
}
