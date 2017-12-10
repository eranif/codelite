//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cppcheck_settings.cpp
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

#include <wx/xrc/xmlres.h>
#include "wx/checklst.h"
#include <memory>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include "cppcheck_settings.h"
#include "plugin.h"
#include <stdio.h>
#include "workspace.h"

// Some macros needed
#ifdef __WXMSW__
#define PIPE_NAME "\\\\.\\pipe\\cppchecker_%s"
#else
#define PIPE_NAME "/tmp/cppchecker.%s.sock"
#endif

//------------------------------------------------------

const wxEventType wxEVT_CPPCHECKJOB_STATUS_MESSAGE = XRCID("cppcheck_status_message");
const wxEventType wxEVT_CPPCHECKJOB_CHECK_COMPLETED = XRCID("cppcheck_check_completed");
const wxEventType wxEVT_CPPCHECKJOB_REPORT = XRCID("cppcheck_report");

//------------------------------------------------------
CppCheckSettings::CppCheckSettings()
    : m_Style(true)
    , m_Performance(true)
    , m_Portability(true)
    , m_UnusedFunctions(true)
    , m_MissingIncludes(true)
    , m_Information(true)
    , m_PosixStandards(true)
    , m_C99Standards(true)
    , m_Cpp11Standards(true)
    , m_Force(false)
    , m_Jobs(1)
    , m_CheckConfig(false)
    , m_saveSuppressedWarnings(false)
    , m_SuppressSystemIncludes(false)
    , m_saveIncludeDirs(false)
{
}

void CppCheckSettings::Serialize(Archive& arch)
{
    arch.Write(wxT("option.style"), m_Style);
    arch.Write(wxT("option.performance"), m_Performance);
    arch.Write(wxT("option.portability"), m_Portability);
    arch.Write(wxT("option.unusedFunctions"), m_UnusedFunctions);
    arch.Write(wxT("option.missingIncludes"), m_MissingIncludes);
    arch.Write(wxT("option.information"), m_Information);
    arch.Write(wxT("option.posixStandards"), m_PosixStandards);
    arch.Write(wxT("option.c99Standards"), m_C99Standards);
    arch.Write(wxT("option.cpp11Standards"), m_Cpp11Standards);
    arch.Write(wxT("option.force"), m_Force);
    arch.Write(wxT("option.jobs"), m_Jobs);
    arch.Write(wxT("m_excludeFiles"), m_excludeFiles);

    if(m_saveSuppressedWarnings) {
        arch.Write(wxT("SuppressedWarningsStrings0"), m_SuppressedWarnings0);
        arch.Write(wxT("SuppressedWarningsStrings1"), m_SuppressedWarnings1);
        // and cache the new values
        m_SuppressedWarningsOrig0.clear();
        m_SuppressedWarningsOrig1.clear();
        m_SuppressedWarningsOrig0.insert(m_SuppressedWarnings0.begin(), m_SuppressedWarnings0.end());
        m_SuppressedWarningsOrig1.insert(m_SuppressedWarnings1.begin(), m_SuppressedWarnings1.end());
    } else {
        // Saving nothing would lose the original values; so save the cached originals
        arch.Write(wxT("SuppressedWarningsStrings0"), m_SuppressedWarningsOrig0);
        arch.Write(wxT("SuppressedWarningsStrings1"), m_SuppressedWarningsOrig1);
    }

    if(m_saveIncludeDirs) {
        arch.Write(wxT("ExtraIncludeDirs"), m_IncludeDirs);
        arch.Write(wxT("SuppressSystemIncludes"), m_SuppressSystemIncludes);
    }
}

void CppCheckSettings::DeSerialize(Archive& arch)
{
    arch.Read(wxT("option.style"), m_Style);
    arch.Read(wxT("option.performance"), m_Performance);
    arch.Read(wxT("option.portability"), m_Portability);
    arch.Read(wxT("option.unusedFunctions"), m_UnusedFunctions);
    arch.Read(wxT("option.missingIncludes"), m_MissingIncludes);
    arch.Read(wxT("option.information"), m_Information);
    arch.Read(wxT("option.posixStandards"), m_PosixStandards);
    arch.Read(wxT("option.c99Standards"), m_C99Standards);
    arch.Read(wxT("option.cpp11Standards"), m_Cpp11Standards);
    arch.Read(wxT("option.force"), m_Force);
    arch.Read(wxT("option.jobs"), m_Jobs);

    arch.Read(wxT("m_excludeFiles"), m_excludeFiles);

    arch.Read(wxT("SuppressedWarningsStrings0"), m_SuppressedWarnings0); // Unchecked ones
    arch.Read(wxT("SuppressedWarningsStrings1"), m_SuppressedWarnings1); // Checked ones

    arch.Read(wxT("ExtraIncludeDirs"), m_IncludeDirs);
    arch.Read(wxT("SuppressSystemIncludes"), m_SuppressSystemIncludes);
    m_saveIncludeDirs = m_IncludeDirs.GetCount() > 0; // If there are saved dirs, this must have been set last time
}

void CppCheckSettings::SetDefaultSuppressedWarnings()
{
    if(m_SuppressedWarnings0.empty() && m_SuppressedWarnings1.empty()) {
        // The first time the Settings dlg is run, none will have been saved; so add the standard ones by hand,
        // unchecked
        m_SuppressedWarnings0.insert(std::pair<wxString, wxString>(wxT("cstyleCast"), wxT("C-style pointer casting")));
        m_SuppressedWarnings0.insert(
            std::pair<wxString, wxString>(wxT("uninitMemberVar"), wxT("...is not initialized in the constructor")));
        m_SuppressedWarnings0.insert(
            std::pair<wxString, wxString>(wxT("variableHidingEnum"), wxT("...hides enumerator with same name")));
        m_SuppressedWarnings0.insert(
            std::pair<wxString, wxString>(wxT("variableScope"), wxT("The scope of the variable...can be reduced")));
    }

    // Now duplicate them, so that the originals can be serialised if the user doesn't want to save any changes
    // Done outside the 'if' statement, as we want it also to happen for deserialised data
    m_SuppressedWarningsOrig0.clear();
    m_SuppressedWarningsOrig1.clear();
    m_SuppressedWarningsOrig0.insert(m_SuppressedWarnings0.begin(), m_SuppressedWarnings0.end());
    m_SuppressedWarningsOrig1.insert(m_SuppressedWarnings1.begin(), m_SuppressedWarnings1.end());
}

void CppCheckSettings::SetSuppressedWarnings(wxCheckListBox* clb, const wxArrayString& keys)
{
    wxCHECK_RET(clb->GetCount() == keys.GetCount(), wxT("Mismatched counts"));

    m_SuppressedWarnings0.clear();
    m_SuppressedWarnings1.clear();
    for(size_t n = 0; n < clb->GetCount(); ++n) {
        AddSuppressedWarning(keys.Item(n), clb->GetString(n), clb->IsChecked(n));
    }
}

void CppCheckSettings::AddSuppressedWarning(const wxString& key, const wxString& label, bool checked)
{
    if(checked) {
        m_SuppressedWarnings1.insert(std::pair<wxString, wxString>(key, label));
    } else {
        m_SuppressedWarnings0.insert(std::pair<wxString, wxString>(key, label));
    }
}

void CppCheckSettings::RemoveSuppressedWarning(const wxString& key)
{
    m_SuppressedWarnings0.erase(key);
    m_SuppressedWarnings1.erase(key);
}

wxString CppCheckSettings::GetOptions() const
{
    wxString options;
    if(GetStyle()) {
        options << wxT(" --enable=style ");
    }
    if(GetPerformance()) {
        options << wxT(" --enable=performance ");
    }
    if(GetPortability()) {
        options << wxT(" --enable=portability ");
    }
    if(GetUnusedFunctions()) {
        options << wxT(" --enable=unusedFunction ");
    }
    if(GetMissingIncludes()) {
        options << wxT(" --enable=missingInclude ");
    }
    if(GetInformation()) {
        options << wxT(" --enable=information ");
    }
    if(GetPosixStandards()) {
        options << wxT(" --std=posix ");
    }
    if(GetC99Standards()) {
        options << wxT(" --std=c99 ");
    }
    if(GetCpp11Standards()) {
        options << wxT(" --std=c++11 ");
    }
    if(GetForce()) {
        options << wxT("--force ");
    }
    if(GetJobs() > 1) {
        options << wxT("-j") << GetJobs() << " ";
    }
    if(GetCheckConfig()) {
        options << wxT("--check-config "); // Though this turns off other checks, afaict it does not harm to emit them
    }

    // Now add any ticked suppressedwarnings
    wxStringMap_t::const_iterator iter = m_SuppressedWarnings1.begin();
    for(; iter != m_SuppressedWarnings1.end(); ++iter) {
        options << wxT(" --suppress=") << (*iter).first << " ";
    }

    // IncludeDirs
    for(size_t n = 0; n < m_IncludeDirs.GetCount(); ++n) {
        wxString item = m_IncludeDirs.Item(n);
        item.Trim().Trim(false);
        if(!item.empty()) {
            options << " -I" << item << " ";
        }
    }
    if(m_SuppressSystemIncludes) {
        options << wxT(" --suppress=") << "missingIncludeSystem ";
    }

    // (Un)Definitions
    for(size_t n = 0; n < m_definitions.GetCount(); ++n) {
        wxString item = m_definitions.Item(n);
        item.Trim().Trim(false);
        if(!item.empty()) {
            options << " -D" << item << " ";
        }
    }
    for(size_t n = 0; n < m_undefines.GetCount(); ++n) {
        wxString item = m_undefines.Item(n);
        item.Trim().Trim(false);
        if(!item.empty()) {
            options << " -U" << item << " ";
        }
    }

    options << wxT(" --template gcc ");
    return options;
}

void CppCheckSettings::LoadProjectSpecificSettings(ProjectPtr project)
{
    wxString rawData;
    wxArrayString definitions, undefines;
    if(project) {
        rawData = project->GetPluginData("CppCheck");
    }

    wxArrayString configurations = wxStringTokenize(rawData, ";", wxTOKEN_RET_EMPTY_ALL);
    if(configurations.GetCount() == 2) { // It'll either be empty or 2
        if(!configurations.Item(0).empty()) {
            definitions = wxStringTokenize(configurations.Item(0), ",");
        }
        if(!configurations.Item(1).empty()) {
            undefines = wxStringTokenize(configurations.Item(1), ",");
        }
    }
    // Note that we set these even if project == NULL, as this will clear any stale values
    SetDefinitions(definitions);
    SetUndefines(undefines);
}
