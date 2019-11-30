//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilerMainPage.h
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

#ifndef COMPILERMAINPAGE_H
#define COMPILERMAINPAGE_H

#include "compiler.h"
#include "compiler_pages.h"
#include <wx/dataview.h>

// =================--------------------
// Helper classes
// =================--------------------
class CompilerPatternDlg : public CompilerPatternDlgBase
{
public:
    CompilerPatternDlg(wxWindow* parent, const wxString& title);
    virtual ~CompilerPatternDlg();

    void SetPattern(const wxString& pattern, const wxString& lineIdx, const wxString& fileIdx,
                    const wxString& columnIndex);

protected:
    virtual void OnSubmit(wxCommandEvent& event);

public:
    wxString GetPattern() const { return m_textPattern->GetValue(); }
    wxString GetFileIndex() const { return m_textFileIndex->GetValue(); }
    wxString GetLineIndex() const { return m_textLineNumber->GetValue(); }
    wxString GetColumnIndex() const { return m_textColumn->GetValue(); }
};

/** Implementing CompilerOptionDlgBase */
class CompilerOptionDialog : public CompilerOptionDlgBase
{
public:
    CompilerOptionDialog(wxWindow* parent, const wxString& title, const wxString& name, const wxString& help,
                         wxWindowID id = wxID_ANY)
        : CompilerOptionDlgBase(parent, id, title)
    {
        m_textCtrl18->ChangeValue(name);
        m_textCtrl19->ChangeValue(help);
    }

    wxString GetName() const { return m_textCtrl18->GetValue(); }

    wxString GetHelp() const { return m_textCtrl19->GetValue(); }
};

class CompilerCompilerOptionDialog : public CompilerOptionDialog
{
public:
    CompilerCompilerOptionDialog(wxWindow* parent, const wxString& name, const wxString& help)
        : CompilerOptionDialog(parent, _("Compiler option"), name, help)
    {
    }
};

class CompilerLinkerOptionDialog : public CompilerOptionDialog
{
public:
    CompilerLinkerOptionDialog(wxWindow* parent, const wxString& name, const wxString& help)
        : CompilerOptionDialog(parent, _("Linker option"), name, help)
    {
    }
};

// ================------------------------------
// Compiler configuration page
// ================------------------------------

class CompilerMainPage : public CompilerMainPageBase
{
    bool m_isDirty;
    CompilerPtr m_compiler;
    wxString m_selSwitchName;
    wxString m_selSwitchValue;
    long m_selectedCmpOption;
    long m_selectedLnkOption;

protected:
    virtual void OnLinkLineActivated(wxDataViewEvent& event);
    virtual void OnLinkerUseFileInput(wxCommandEvent& event);
    virtual void OnAddExistingCompiler(wxCommandEvent& event);
    virtual void OnCloneCompiler(wxCommandEvent& event);
    virtual void OnScanCompilers(wxCommandEvent& event);
    virtual void OnCmdModify(wxCommandEvent& event);
    virtual void OnValueChanged(wxPropertyGridEvent& event);
    virtual void OnRenameCompiler(wxCommandEvent& event);
    virtual void OnDeleteCompiler(wxCommandEvent& event);
    virtual void OnContextMenu(wxContextMenuEvent& event);
    virtual void OnCompilerSelected(wxCommandEvent& event);
    void Initialize();

    // Tools
    void InitializeTools();
    void SaveTools();

    // Patterns
    void InitializePatterns();
    void SavePatterns();
    void DoUpdateErrPattern(long item);
    void DoUpdateWarnPattern(long item);

    // Compiler Switches
    void AddSwitch(const wxString& name, const wxString& value, bool choose);
    void EditSwitch();
    void SaveSwitches();
    void InitializeSwitches();

    // File Types
    void InitialiseTemplates();
    void SaveTemplates();

    // Advanced page
    void InitializeAdvancePage();
    void SaveAdvancedPage();

    // Compiler options
    void InitializeCompilerOptions();
    void SaveComilerOptions();

    // Compiler options
    void InitializeLinkerOptions();
    void SaveLinkerOptions();

    void LoadCompiler(const wxString& compilerName);
    void DoFileTypeActivated(const wxDataViewItem& item);
    
public:
    CompilerMainPage(wxWindow* parent);
    virtual ~CompilerMainPage();
    void LoadCompilers();
    void Save();

    bool IsDirty() const { return m_isDirty; }

protected:
    virtual void OnBtnAddErrPattern(wxCommandEvent& event);
    virtual void OnBtnAddWarnPattern(wxCommandEvent& event);
    virtual void OnBtnDelErrPattern(wxCommandEvent& event);
    virtual void OnBtnDelWarnPattern(wxCommandEvent& event);
    virtual void OnBtnUpdateErrPattern(wxCommandEvent& event);
    virtual void OnBtnUpdateWarnPattern(wxCommandEvent& event);
    virtual void OnCompilerOptionActivated(wxListEvent& event);
    virtual void OnCompilerOptionDeSelected(wxListEvent& event);
    virtual void OnCompilerOptionSelected(wxListEvent& event);
    virtual void OnCustomEditorButtonClicked(wxCommandEvent& event);
    virtual void OnDeleteCompilerOption(wxCommandEvent& event);
    virtual void OnDeleteFileType(wxCommandEvent& event);
    virtual void OnDeleteLinkerOption(wxCommandEvent& event);
    virtual void OnEditIncludePaths(wxCommandEvent& event);
    virtual void OnEditLibraryPaths(wxCommandEvent& event);
    virtual void OnErrItemActivated(wxListEvent& event);
    virtual void OnErrorPatternSelectedUI(wxUpdateUIEvent& event);
    virtual void OnFileTypeActivated(wxDataViewEvent& event);
    virtual void OnItemActivated(wxListEvent& event);
    virtual void OnItemSelected(wxListEvent& event);
    virtual void OnLinkerOptionActivated(wxListEvent& event);
    virtual void OnLinkerOptionDeSelected(wxListEvent& event);
    virtual void OnLinkerOptionSelected(wxListEvent& event);
    virtual void OnNewCompilerOption(wxCommandEvent& event);
    virtual void OnNewFileType(wxCommandEvent& event);
    virtual void OnNewLinkerOption(wxCommandEvent& event);
    virtual void OnWarnItemActivated(wxListEvent& event);
    virtual void OnWarningPatternSelectedUI(wxUpdateUIEvent& event);
};
#endif // COMPILERMAINPAGE_H
