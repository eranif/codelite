//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cppcheck_settings.h
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

#ifndef __cppcheckjob__
#define __cppcheckjob__

#include "job.h"
#include "serialized_object.h"
#include "project.h"
#include "macros.h"

// Define the events needed by this job
extern const wxEventType wxEVT_CPPCHECKJOB_STATUS_MESSAGE;
extern const wxEventType wxEVT_CPPCHECKJOB_CHECK_COMPLETED;
extern const wxEventType wxEVT_CPPCHECKJOB_REPORT;

class IPlugin;
class wxCheckListBox;

class CppCheckSettings : public SerializedObject
{
    bool m_Style;
    bool m_Performance;
    bool m_Portability;
    bool m_UnusedFunctions;
    bool m_MissingIncludes;
    bool m_Information;
    bool m_PosixStandards;
    bool m_C99Standards;
    bool m_Cpp11Standards;
    bool m_Force;
    int m_Jobs;
    bool m_CheckConfig;
    wxArrayString m_excludeFiles;
    wxStringMap_t m_SuppressedWarnings0;     // The items unchecked in the checklistbox
    wxStringMap_t m_SuppressedWarnings1;     // The checked ones
    wxStringMap_t m_SuppressedWarningsOrig0; // Ditto, containing the original values
    wxStringMap_t m_SuppressedWarningsOrig1;
    bool m_saveSuppressedWarnings;
    wxArrayString m_IncludeDirs;
    bool m_SuppressSystemIncludes;
    bool m_saveIncludeDirs;
    wxArrayString m_definitions;
    wxArrayString m_undefines;

public:
    CppCheckSettings();

    bool GetStyle() const { return m_Style; }
    bool GetPerformance() const { return m_Performance; }
    bool GetPortability() const { return m_Portability; }
    bool GetUnusedFunctions() const { return m_UnusedFunctions; }
    bool GetMissingIncludes() const { return m_MissingIncludes; }
    bool GetInformation() const { return m_Information; }
    bool GetPosixStandards() const { return m_PosixStandards; }
    bool GetC99Standards() const { return m_C99Standards; }
    bool GetCpp11Standards() const { return m_Cpp11Standards; }
    bool GetForce() const { return m_Force; }
    int GetJobs() const { return m_Jobs; }
    bool GetCheckConfig() const { return m_CheckConfig; }
    const wxArrayString& GetExcludeFiles() const { return m_excludeFiles; }
    const wxStringMap_t* GetSuppressedWarningsStrings0() const { return &m_SuppressedWarnings0; }
    const wxStringMap_t* GetSuppressedWarningsStrings1() const { return &m_SuppressedWarnings1; }
    const wxArrayString& GetIncludeDirs() const { return m_IncludeDirs; }
    bool GetSuppressSystemIncludes() const { return m_SuppressSystemIncludes; }
    bool GetSaveIncludeDirs() const { return m_saveIncludeDirs; }
    const wxArrayString& GetDefinitions() const { return m_definitions; }
    const wxArrayString& GetUndefines() const { return m_undefines; }

    void SetStyle(bool Style) { m_Style = Style; }
    void SetPerformance(bool Performance) { m_Performance = Performance; }
    void SetPortability(bool Portability) { m_Portability = Portability; }
    void SetUnusedFunctions(bool UnusedFunctions) { m_UnusedFunctions = UnusedFunctions; }
    void SetMissingIncludes(bool MissingIncludes) { m_MissingIncludes = MissingIncludes; }
    void SetInformation(bool Information) { m_Information = Information; }
    void SetPosixStandards(bool PosixStandards) { m_PosixStandards = PosixStandards; }
    void SetC99Standards(bool C99Standards) { m_C99Standards = C99Standards; }
    void SetCpp11Standards(bool Cpp11Standards) { m_Cpp11Standards = Cpp11Standards; }
    void SetForce(bool Force) { m_Force = Force; }
    void SetJobs(int jobs) { m_Jobs = jobs; }
    void SetCheckConfig(bool checkconfig) { m_CheckConfig = checkconfig; }
    void SetExcludeFiles(const wxArrayString& excludeFiles) { m_excludeFiles = excludeFiles; }
    void AddSuppressedWarning(const wxString& key, const wxString& label, bool checked);
    void RemoveSuppressedWarning(const wxString& key);
    void SetSuppressedWarnings(wxCheckListBox* clb, const wxArrayString& keys);
    void SetSaveSuppressedWarnings(bool save) { m_saveSuppressedWarnings = save; }
    void SetDefaultSuppressedWarnings();

    void SetIncludeDirs(const wxArrayString& dirs) { m_IncludeDirs = dirs; }
    void SetSuppressSystemIncludes(bool suppress) { m_SuppressSystemIncludes = suppress; }
    void SetSaveIncludeDirs(bool save) { m_saveIncludeDirs = save; }

    void SetDefinitions(const wxArrayString& definitions) { m_definitions = definitions; }
    void SetUndefines(const wxArrayString& undefines) { m_undefines = undefines; }

    virtual void Serialize(Archive& arch);
    virtual void DeSerialize(Archive& arch);

    wxString GetOptions() const;
    void LoadProjectSpecificSettings(ProjectPtr proj);
};

#endif // __cppcheckjob__
