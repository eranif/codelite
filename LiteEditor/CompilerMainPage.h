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
#include "compiler_pages.hpp"

#include <wx/dataview.h>

// =================--------------------
// Helper classes
// =================--------------------
class CompilerPatternDlg : public CompilerPatternDlgBase
{
public:
    CompilerPatternDlg(wxWindow* parent, const wxString& title);
    ~CompilerPatternDlg() override = default;

    void
    SetPattern(const wxString& pattern, const wxString& lineIdx, const wxString& fileIdx, const wxString& columnIndex);

protected:
    void OnSubmit(wxCommandEvent& event) override;

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
    CompilerOptionDialog(
        wxWindow* parent, const wxString& title, const wxString& name, const wxString& help, wxWindowID id = wxID_ANY)
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
    void OnLinkLineActivated(wxDataViewEvent& event) override;
    void OnLinkerUseFileInput(wxCommandEvent& event) override;
    virtual void OnAddExistingCompiler(wxCommandEvent& event);
    virtual void OnCloneCompiler(wxCommandEvent& event);
    virtual void OnScanCompilers(wxCommandEvent& event);
    void OnCmdModify(wxCommandEvent& event) override;
    void OnValueChanged(wxPropertyGridEvent& event) override;
    virtual void OnRenameCompiler(wxCommandEvent& event);
    virtual void OnDeleteCompiler(wxCommandEvent& event);
    void OnContextMenu(wxContextMenuEvent& event) override;
    void OnCompilerSelected(wxCommandEvent& event) override;
    void Initialize();

    // Tools
    void InitializeTools();
    void SaveTools();

    // Patterns
    void InitializePatterns();
    void SavePatterns();
    void DoUpdateErrPattern(const wxDataViewItem& item);
    void DoUpdateWarnPattern(const wxDataViewItem& item);
    void DoUpdatePattern(clThemedListCtrl* list, const wxDataViewItem& item, const wxString& dialog_title);
    void DoAddPattern(clThemedListCtrl* list, const Compiler::CmpInfoPattern& pattern);

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
    void SaveCompilerOptions();

    // Compiler options
    void InitializeLinkerOptions();
    void SaveLinkerOptions();

    void LoadCompiler(const wxString& compilerName);
    void DoFileTypeActivated(const wxDataViewItem& item);

public:
    CompilerMainPage(wxWindow* parent);
    ~CompilerMainPage() override = default;
    void LoadCompilers();
    void Save();

    bool IsDirty() const { return m_isDirty; }

protected:
    void OnBtnAddErrPattern(wxCommandEvent& event) override;
    void OnBtnAddWarnPattern(wxCommandEvent& event) override;
    void OnBtnDelErrPattern(wxCommandEvent& event) override;
    void OnBtnDelWarnPattern(wxCommandEvent& event) override;
    void OnBtnUpdateErrPattern(wxCommandEvent& event) override;
    void OnBtnUpdateWarnPattern(wxCommandEvent& event) override;
    void OnCompilerOptionActivated(wxListEvent& event) override;
    void OnCompilerOptionDeSelected(wxListEvent& event) override;
    void OnCompilerOptionSelected(wxListEvent& event) override;
    void OnCustomEditorButtonClicked(wxCommandEvent& event) override;
    void OnDeleteCompilerOption(wxCommandEvent& event) override;
    void OnDeleteFileType(wxCommandEvent& event) override;
    void OnDeleteLinkerOption(wxCommandEvent& event) override;
    void OnEditIncludePaths(wxCommandEvent& event) override;
    void OnEditLibraryPaths(wxCommandEvent& event) override;
    void OnErrItemActivated(wxDataViewEvent& event) override;
    void OnErrorPatternSelectedUI(wxUpdateUIEvent& event) override;
    void OnFileTypeActivated(wxDataViewEvent& event) override;
    void OnItemActivated(wxListEvent& event) override;
    void OnItemSelected(wxListEvent& event) override;
    void OnLinkerOptionActivated(wxListEvent& event) override;
    void OnLinkerOptionDeSelected(wxListEvent& event) override;
    void OnLinkerOptionSelected(wxListEvent& event) override;
    void OnNewCompilerOption(wxCommandEvent& event) override;
    void OnNewFileType(wxCommandEvent& event) override;
    void OnNewLinkerOption(wxCommandEvent& event) override;
    void OnWarnItemActivated(wxDataViewEvent& event) override;
    void OnWarningPatternSelectedUI(wxUpdateUIEvent& event) override;
};
#endif // COMPILERMAINPAGE_H
