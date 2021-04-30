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
#include "clTempFile.hpp"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include <wx/dir.h>
#include <wx/fileconf.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>

GCCMetadata::GCCMetadata(const wxString& basename)
    : m_basename(basename)
{
}

GCCMetadata::~GCCMetadata() {}

void GCCMetadata::Load(const wxString& tool, const wxString& rootDir, bool is_cygwin)
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
    
#ifdef __WXMSW__
    if(::clIsCygwinEnvironment()) {
        command << tool << " -v -x c++ /dev/null -fsyntax-only";
    } else {
        wxFileName cxx(tool);

        // execute the command from the tool directory
        working_directory = cxx.GetPath();
        ::WrapWithQuotes(working_directory);
        command << "cd " << working_directory << " && " << tool << " -xc++ -E -v " << tmpfile.GetFullPath(true)
                << " -fsyntax-only";
    }
#else
    command << tool << " -v -x c++ /dev/null -fsyntax-only";
#endif

    wxString outputStr = RunCommand(command, working_directory);

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
    m_target = RunCommand(tool + " -dumpmachine", wxFileName(tool).GetPath());
    wxString versionString = RunCommand(tool + " -dumpversion", wxFileName(tool).GetPath());
    m_name << m_basename << "-" << versionString;
}

wxString GCCMetadata::RunCommand(const wxString& command, const wxString& working_directory)
{
    clDEBUG() << "Running command:" << command << endl;
    wxString outputStr;
    IProcess::Ptr_t proc(::CreateSyncProcess(command, IProcessCreateDefault | IProcessWrapInShell, working_directory));
    if(proc) {
        proc->WaitForTerminate(outputStr);
    }
    clDEBUG() << "Output is:" << outputStr << endl;
    outputStr.Trim().Trim(false);
    return outputStr;
}
