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

#include "editor_config.h"
#include "environmentconfig.h"
#include "fileutils.h"
#include "globals.h"
#include "includepathlocator.h"
#include "procutils.h"
#include <wx/dir.h>
#include <wx/fileconf.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>

// Helper method
static wxArrayString ExecCommand(const wxString& cmd)
{
    wxArrayString outputArr;
    EnvSetter setter;
    ProcUtils::SafeExecuteCommand(cmd, outputArr);
    return outputArr;
}

IncludePathLocator::IncludePathLocator(IManager* mgr)
    : m_mgr(mgr)
{
}

IncludePathLocator::~IncludePathLocator() {}

void IncludePathLocator::Locate(wxArrayString& paths, wxArrayString& excludePaths, bool thirdPartyLibs,
                                const wxString& tool)
{
    // Common compiler paths - should be placed at top of the include path!
    wxString tmpfile1 = wxFileName::CreateTempFileName(wxT("codelite"));
    wxString command;
    wxString tmpfile = tmpfile1;
    tmpfile += wxT(".cpp");

    wxString bin = tool;
    if(bin.IsEmpty()) { bin = wxT("gcc"); }

    wxRenameFile(tmpfile1, tmpfile);

    // GCC prints parts of its output to stdout and some to stderr
    // redirect all output to stdout
#if defined(__WXMAC__) || defined(__WXGTK__)
    // Mac does not like the standard command
    command = wxString::Format(wxT("%s -v -x c++ /dev/null -fsyntax-only"), bin.c_str());
#else
    command = wxString::Format(wxT("%s -v -x c++ %s -fsyntax-only"), bin.c_str(), tmpfile.c_str());
#endif

    wxString outputStr = wxShellExec(command, wxEmptyString);
    clRemoveFile(tmpfile);

    wxArrayString outputArr = wxStringTokenize(outputStr, wxT("\n\r"), wxTOKEN_STRTOK);
    // Analyze the output
    bool collect(false);
    for(size_t i = 0; i < outputArr.GetCount(); i++) {
        if(outputArr[i].Contains(wxT("#include <...> search starts here:"))) {
            collect = true;
            continue;
        }

        if(outputArr[i].Contains(wxT("End of search list."))) { break; }

        if(collect) {

            wxString file = outputArr.Item(i).Trim().Trim(false);

            // on Mac, (framework directory) appears also,
            // but it is harmless to use it under all OSs
            file.Replace(wxT("(framework directory)"), wxT(""));
            file.Trim().Trim(false);

            wxFileName includePath(file, wxT(""));
            includePath.Normalize();

            paths.Add(includePath.GetPath());
        }
    }

    if(thirdPartyLibs) {
        // try to locate QMAKE
        wxFileConfig qmakeConf(wxEmptyString, wxEmptyString, m_mgr->GetStartupDirectory() + wxT("/config/qmake.ini"));
        wxString groupName;
        long index(0);
        wxArrayString out;
        wxString qmake(wxT("qmake"));

        if(qmakeConf.GetFirstGroup(groupName, index)) {
            // we got qmake configuration, use it instead of the default qmake command
            qmake = qmakeConf.Read(groupName + wxT("/qmake"));
        }

        // Run: qmake -query QT_INSTALL_PREFIX
        wxString cmd;
        cmd << qmake << wxT(" -query QT_INSTALL_PREFIX");

#ifdef __WXGTK__
        cmd << wxT(" 2>/dev/null");
#endif

        out = ExecCommand(cmd);

        if(out.IsEmpty() == false) {

            wxString qt_output(out.Item(0));
            qt_output.Trim().Trim(false);

#if defined(__WXGTK__) || defined(__WXMAC__)
            wxString pathQt4, pathQt3, pathQt;
            pathQt4 << qt_output << wxFileName::GetPathSeparator() << wxT("include") << wxFileName::GetPathSeparator()
                    << wxT("qt4");
            pathQt3 << qt_output << wxFileName::GetPathSeparator() << wxT("include") << wxFileName::GetPathSeparator()
                    << wxT("qt3");
            pathQt << qt_output << wxFileName::GetPathSeparator() << wxT("include");

            if(wxDir::Exists(pathQt4)) {
                wxString tmpPath;

                tmpPath = pathQt4 + wxT("/QtCore");
                if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

                tmpPath = pathQt4 + wxT("/QtGui");
                if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

                tmpPath = pathQt4 + wxT("/QtXml");
                if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

            } else if(wxDir::Exists(pathQt3)) {

                wxString tmpPath;

                tmpPath = pathQt3 + wxT("/QtCore");
                if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

                tmpPath = pathQt3 + wxT("/QtGui");
                if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

                tmpPath = pathQt3 + wxT("/QtXml");
                if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

            } else if(wxDir::Exists(pathQt)) {

                wxString tmpPath;

                tmpPath = pathQt + wxT("/QtCore");
                if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

                tmpPath = pathQt + wxT("/QtGui");
                if(wxFileName::DirExists(tmpPath)) paths.Add(pathQt);

                tmpPath = pathQt + wxT("/QtXml");
                if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);
            }

#else // __WXMSW__
            wxString pathWin;
            pathWin << qt_output << wxFileName::GetPathSeparator() << wxT("include") << wxFileName::GetPathSeparator();
            if(wxDir::Exists(pathWin)) {

                wxString tmpPath;

                tmpPath = pathWin + wxT("QtCore");
                if(wxFileName::DirExists(tmpPath)) {
                    wxFileName fn(tmpPath, wxT(""));
                    paths.Add(fn.GetPath());
                }

                tmpPath = pathWin + wxT("QtGui");
                if(wxFileName::DirExists(tmpPath)) {
                    wxFileName fn(tmpPath, wxT(""));
                    paths.Add(fn.GetPath());
                }

                tmpPath = pathWin + wxT("QtXml");
                if(wxFileName::DirExists(tmpPath)) {
                    wxFileName fn(tmpPath, wxT(""));
                    paths.Add(fn.GetPath());
                }
            }
#endif
        }

        // Try wxWidgets
#ifdef __WXMSW__
        // On Windows, just read the content of the WXWIN environment variable
        wxString wxwin;
        if(wxGetEnv(wxT("WX_INCL_HOME"), &wxwin)) {
            // we got the path to the installation of wxWidgets
            if(wxDir::Exists(wxwin)) {
                paths.Add(wxwin);
                excludePaths.Add(wxwin + wxT("\\univ"));
                excludePaths.Add(wxwin + wxT("\\unix"));
            }
        }
#else
        // run wx-config and parse the output
        out.Clear();
        out = ExecCommand(wxT("wx-config --cxxflags 2>/dev/null"));
        if(out.IsEmpty() == false) {
            wxString line(out.Item(0));
            int where = line.Find(wxT(" -I"));
            while(where != wxNOT_FOUND) {
                line = line.Mid(where + 3);
                paths.Add(line.BeforeFirst(wxT(' ')));
                where = line.Find(wxT(" -I"));
            }
        }
#endif
    }
}
