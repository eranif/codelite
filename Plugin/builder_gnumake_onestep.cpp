//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009
// file name            : builder_gnumake_onestep.cpp
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
#include "project.h"
#include <wx/app.h>
#include <wx/msgdlg.h>

#include "editor_config.h"
#include <wx/stopwatch.h>
#include "environmentconfig.h"
#include "evnvarlist.h"
#include "environmentconfig.h"
#include "builder_gnumake_onestep.h"
#include "configuration_mapping.h"
#include "dirsaver.h"
#include "wx/tokenzr.h"
#include "macros.h"
#include "build_settings_config.h"
#include "buildmanager.h"
#include "wx/sstream.h"
#include "globals.h"

// wxStopWatch g_sw;

static bool OS_WINDOWS = wxGetOsVersion() & wxOS_WINDOWS ? true : false;

#if PERFORMANCE
#define TIMER_START() \
    {                 \
        g_sw.Start(); \
    }

#define PRINT_TIMESTAMP(msg)                                     \
    {                                                            \
        wxString log_msg(msg);                                   \
        wxPrintf(wxT("%08d: %s"), g_sw.Time(), log_msg.c_str()); \
    }
#else
#define TIMER_START()
#define PRINT_TIMESTAMP(msg)
#endif

BuilderGnuMakeOneStep::BuilderGnuMakeOneStep()
    : BuilderGNUMakeClassic(wxT("GNU makefile onestep build"), wxT("make"), wxT("-f"))
{
}

BuilderGnuMakeOneStep::~BuilderGnuMakeOneStep() {}

void BuilderGnuMakeOneStep::CreateListMacros(ProjectPtr proj, const wxString& confToBuild, wxString& text)
{
    // create a list of Sources
    BuilderGNUMakeClassic::CreateSrcList(proj, confToBuild, text);
    // create a list of objects
    BuilderGNUMakeClassic::CreateObjectList(proj, confToBuild, text);
}

void BuilderGnuMakeOneStep::CreateLinkTargets(
    const wxString& type, BuildConfigPtr bldConf, wxString& text, wxString& targetName)
{
    // specify outfile dependency directly on source files (only)
    text << wxT("all: $(OutputFile)\n\n");
    text << wxT("$(OutputFile): makeDirStep $(Srcs)\n");
    targetName = wxT("makeDirStep");
    CreateTargets(type, bldConf, text); // overridden CreateTargets

    // if (bldConf->IsLinkerRequired()) {
    //	CreateTargets(type, bldConf, text);
    //}
}

void BuilderGnuMakeOneStep::CreateFileTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text)
{
    // override to do do nothing (no link objects) build rule already given in CreateLinkTargets
}

void BuilderGnuMakeOneStep::CreateTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text)
{
    if(OS_WINDOWS) {
        text << wxT("\t") << wxT("@makedir $(@D)\n");
    } else {
        text << wxT("\t") << wxT("@mkdir -p $(@D)\n");
    }

    // these patterns below should be all set and loadable from configuration file.
    // compilerName is tool that does single stage construction srcs -> exe/lib

    if(type == PROJECT_TYPE_STATIC_LIBRARY) {
        // create a static library
        text << wxT("\t")
             << wxT("$(CXX) $(ArchiveOutputSwitch) $(OutputSwitch)$(OutputFile) $(Libs) $(CXXFLAGS) $(Srcs)\n");
    } else if(type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
        // create a shared library
        text << wxT("\t") << wxT("$(CXX) $(ObjectSwitch) $(OutputSwitch)$(OutputFile) $(Libs) $(CXXFLAGS) $(Srcs)\n");
    } else if(type == PROJECT_TYPE_EXECUTABLE) {
        // create an executable
        text << wxT("\t") << wxT("$(CXX) $(SourceSwitch) $(OutputSwitch)$(OutputFile) $(Libs) $(CXXFLAGS) $(Srcs)\n");
    }
}
