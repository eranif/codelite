//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilerMainPage.cpp
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

#include "CompilerMainPage.h"
#include "compiler.h"
#include <build_settings_config.h>
#include <globals.h>
#include <windowattrmanager.h>
#include "editcmpfileinfodlg.h"
#include <EditDlg.h>
#include <event_notifier.h>
#include <workspace.h>
#include <build_config.h>
#include <cl_aui_notebook_art.h>
#include <wx/wupdlock.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include "advanced_settings.h"
#include <wx/menu.h>
#include <wx/textdlg.h>

CompilerMainPage::CompilerMainPage(wxWindow* parent)
    : CompilerMainPageBase(parent)
    , m_isDirty(false)
    , m_compiler(NULL)
    , m_selectedFileType(-1)
    , m_selectedCmpOption(-1)
    , m_selectedLnkOption(-1)
{
    // =============-----------------------------
    // Patterns page initialization
    // =============-----------------------------
    m_listErrPatterns->InsertColumn(0, _("Pattern"));
    m_listErrPatterns->InsertColumn(1, _("File name index"));
    m_listErrPatterns->InsertColumn(2, _("Line number index"));
    m_listErrPatterns->InsertColumn(3, _("Column index"));

    m_listErrPatterns->SetColumnWidth(0, 200);
    m_listErrPatterns->SetColumnWidth(1, 50);
    m_listErrPatterns->SetColumnWidth(2, 50);
    m_listErrPatterns->SetColumnWidth(3, 50);

    m_listWarnPatterns->InsertColumn(0, _("Pattern"));
    m_listWarnPatterns->InsertColumn(1, _("File name index"));
    m_listWarnPatterns->InsertColumn(2, _("Line number index"));
    m_listWarnPatterns->InsertColumn(3, _("Column index"));

    m_listWarnPatterns->SetColumnWidth(0, 200);
    m_listWarnPatterns->SetColumnWidth(1, 50);
    m_listWarnPatterns->SetColumnWidth(2, 50);
    m_listWarnPatterns->SetColumnWidth(3, 50);

    // ==============------------------
    // Switches
    // ==============------------------
    m_listSwitches->InsertColumn(0, _("Name"));
    m_listSwitches->InsertColumn(1, _("Value"));

    // =============----------------------
    // File Types
    // =============----------------------
    m_listCtrlFileTypes->InsertColumn(0, _("Extension"));
    m_listCtrlFileTypes->InsertColumn(1, _("Kind"));
    m_listCtrlFileTypes->InsertColumn(2, _("Compilation Line"));

    // =====---------------------------------
    // Compiler options
    // =====---------------------------------
    m_listCompilerOptions->InsertColumn(0, _("Switch"));
    m_listCompilerOptions->InsertColumn(1, _("Help"));

    // =====---------------------------------
    // Linker options
    // =====---------------------------------
    m_listLinkerOptions->InsertColumn(0, _("Switch"));
    m_listLinkerOptions->InsertColumn(1, _("Help"));
}

CompilerMainPage::~CompilerMainPage() {}

void CompilerMainPage::OnBtnAddErrPattern(wxCommandEvent& event)
{
    CompilerPatternDlg dlg(wxGetTopLevelParent(this), _("Add compiler error pattern"));
    if(dlg.ShowModal() == wxID_OK) {
        long item = AppendListCtrlRow(m_listErrPatterns);
        SetColumnText(m_listErrPatterns, item, 0, dlg.GetPattern());
        SetColumnText(m_listErrPatterns, item, 1, dlg.GetFileIndex());
        SetColumnText(m_listErrPatterns, item, 2, dlg.GetLineIndex());
        SetColumnText(m_listErrPatterns, item, 3, dlg.GetColumnIndex());
        m_isDirty = true;
    }
}

void CompilerMainPage::OnBtnAddWarnPattern(wxCommandEvent& event)
{
    CompilerPatternDlg dlg(wxGetTopLevelParent(this), _("Add compiler warning pattern"));
    if(dlg.ShowModal() == wxID_OK) {
        m_isDirty = true;
        long item = AppendListCtrlRow(m_listWarnPatterns);
        SetColumnText(m_listWarnPatterns, item, 0, dlg.GetPattern());
        SetColumnText(m_listWarnPatterns, item, 1, dlg.GetFileIndex());
        SetColumnText(m_listWarnPatterns, item, 2, dlg.GetLineIndex());
        SetColumnText(m_listWarnPatterns, item, 3, dlg.GetColumnIndex());
    }
}

void CompilerMainPage::OnBtnDelErrPattern(wxCommandEvent& event)
{
    int sel = m_listErrPatterns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(sel == wxNOT_FOUND) {
        return;
    }
    m_listErrPatterns->DeleteItem(sel);
    m_isDirty = true;
}

void CompilerMainPage::OnBtnDelWarnPattern(wxCommandEvent& event)
{
    int sel = m_listWarnPatterns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(sel == wxNOT_FOUND) {
        return;
    }
    m_listWarnPatterns->DeleteItem(sel);
    m_isDirty = true;
}

void CompilerMainPage::OnBtnUpdateErrPattern(wxCommandEvent& event)
{
    int sel = m_listErrPatterns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(sel == wxNOT_FOUND) {
        return;
    }
    DoUpdateErrPattern(sel);
    m_isDirty = true;
}

void CompilerMainPage::OnBtnUpdateWarnPattern(wxCommandEvent& event)
{
    int sel = m_listWarnPatterns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(sel == wxNOT_FOUND) {
        return;
    }
    DoUpdateWarnPattern(sel);
    m_isDirty = true;
}

void CompilerMainPage::OnCompilerOptionActivated(wxListEvent& event)
{
    if(m_selectedCmpOption == wxNOT_FOUND) {
        return;
    }

    wxString name = m_listCompilerOptions->GetItemText(m_selectedCmpOption);
    wxString help = GetColumnText(m_listCompilerOptions, m_selectedCmpOption, 1);
    CompilerCompilerOptionDialog dlg(wxGetTopLevelParent(this), name, help);
    if(dlg.ShowModal() == wxID_OK) {
        SetColumnText(m_listCompilerOptions, m_selectedCmpOption, 0, dlg.GetName());
        SetColumnText(m_listCompilerOptions, m_selectedCmpOption, 1, dlg.GetHelp());
        m_listCompilerOptions->SetColumnWidth(1, wxLIST_AUTOSIZE);
        m_isDirty = true;
    }
}

void CompilerMainPage::OnCompilerOptionDeSelected(wxListEvent& event)
{
#ifndef __WXMAC__
    m_selectedCmpOption = wxNOT_FOUND;
#endif
    event.Skip();
}

void CompilerMainPage::OnCompilerOptionSelected(wxListEvent& event)
{
    m_selectedCmpOption = event.m_itemIndex;
    event.Skip();
}

void CompilerMainPage::OnCustomEditorButtonClicked(wxCommandEvent& event)
{
    wxPGProperty* prop = m_pgMgrTools->GetSelectedProperty();
    CHECK_PTR_RET(prop);

    wxString oldValue = prop->GetValueAsString();
    wxFileName fn(oldValue);

    wxString newPath = ::wxFileSelector(_("Select a file"), fn.GetPath());
    if(!newPath.IsEmpty()) {
        ::WrapWithQuotes(newPath);
        prop->SetValueFromString(newPath);
    }
    m_isDirty = true;
}

void CompilerMainPage::OnDeleteCompilerOption(wxCommandEvent& event)
{
    if(m_selectedCmpOption != wxNOT_FOUND) {
        if(wxMessageBox(_("Are you sure you want to delete this compiler option?"),
                        _("CodeLite"),
                        wxYES_NO | wxCANCEL) == wxYES) {
            m_listCompilerOptions->DeleteItem(m_selectedCmpOption);
            m_listCompilerOptions->SetColumnWidth(1, wxLIST_AUTOSIZE);
            m_selectedCmpOption = wxNOT_FOUND;
            m_isDirty = true;
        }
    }
}

void CompilerMainPage::OnDeleteFileType(wxCommandEvent& event)
{
    if(m_selectedFileType != wxNOT_FOUND) {
        if(wxMessageBox(_("Are you sure you want to delete this file type?"), _("CodeLite"), wxYES_NO | wxCANCEL) ==
           wxYES) {
            m_listCtrlFileTypes->DeleteItem(m_selectedFileType);
            m_selectedFileType = wxNOT_FOUND;
            m_isDirty = true;
        }
    }
}

void CompilerMainPage::OnDeleteLinkerOption(wxCommandEvent& event)
{
    if(m_selectedLnkOption != wxNOT_FOUND) {
        if(wxMessageBox(_("Are you sure you want to delete this linker option?"), _("CodeLite"), wxYES_NO | wxCANCEL) ==
           wxYES) {
            m_isDirty = true;
            m_listLinkerOptions->DeleteItem(m_selectedLnkOption);
            m_listLinkerOptions->SetColumnWidth(1, wxLIST_AUTOSIZE);
            m_selectedLnkOption = wxNOT_FOUND;
        }
    }
}

void CompilerMainPage::OnEditIncludePaths(wxCommandEvent& event)
{
    wxString curIncludePath = m_textCtrlGlobalIncludePath->GetValue();
    curIncludePath = wxJoin(::wxStringTokenize(curIncludePath, ";", wxTOKEN_STRTOK), '\n', '\0');
    wxString newIncludePath = ::clGetStringFromUser(curIncludePath, EventNotifier::Get()->TopFrame());
    newIncludePath.Trim().Trim(false);
    if(!newIncludePath.IsEmpty()) {
        m_isDirty = true;
        newIncludePath = wxJoin(::wxStringTokenize(newIncludePath, "\n\r", wxTOKEN_STRTOK), ';', '\0');
        m_textCtrlGlobalIncludePath->ChangeValue(newIncludePath);
    }
}

void CompilerMainPage::OnEditLibraryPaths(wxCommandEvent& event)
{
    wxString curLibPath = m_textCtrlGlobalLibPath->GetValue();
    curLibPath = wxJoin(::wxStringTokenize(curLibPath, ";", wxTOKEN_STRTOK), '\n', '\0');
    wxString newLibPath = ::clGetStringFromUser(curLibPath, EventNotifier::Get()->TopFrame());
    newLibPath.Trim().Trim(false);
    if(!newLibPath.IsEmpty()) {
        m_isDirty = true;
        newLibPath = wxJoin(::wxStringTokenize(newLibPath, "\n\r", wxTOKEN_STRTOK), ';', '\0');
        m_textCtrlGlobalLibPath->ChangeValue(newLibPath);
    }
}

void CompilerMainPage::OnErrItemActivated(wxListEvent& event)
{
    DoUpdateErrPattern(event.GetIndex());
    m_isDirty = true;
}

void CompilerMainPage::OnErrorPatternSelectedUI(wxUpdateUIEvent& event)
{
    int sel = m_listErrPatterns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    event.Enable(sel != wxNOT_FOUND);
}

void CompilerMainPage::DoUpdateWarnPattern(long item)
{
    wxString pattern = GetColumnText(m_listWarnPatterns, item, 0);
    wxString fileIdx = GetColumnText(m_listWarnPatterns, item, 1);
    wxString lineIdx = GetColumnText(m_listWarnPatterns, item, 2);
    wxString colIdx = GetColumnText(m_listWarnPatterns, item, 3);
    CompilerPatternDlg dlg(wxGetTopLevelParent(this), _("Update compiler warning pattern"));
    dlg.SetPattern(pattern, lineIdx, fileIdx, colIdx);
    if(dlg.ShowModal() == wxID_OK) {
        SetColumnText(m_listWarnPatterns, item, 0, dlg.GetPattern());
        SetColumnText(m_listWarnPatterns, item, 1, dlg.GetFileIndex());
        SetColumnText(m_listWarnPatterns, item, 2, dlg.GetLineIndex());
        SetColumnText(m_listWarnPatterns, item, 3, dlg.GetColumnIndex());
    }
}

void CompilerMainPage::DoUpdateErrPattern(long item)
{
    wxString pattern = GetColumnText(m_listErrPatterns, item, 0);
    wxString fileIdx = GetColumnText(m_listErrPatterns, item, 1);
    wxString lineIdx = GetColumnText(m_listErrPatterns, item, 2);
    wxString colIdx = GetColumnText(m_listErrPatterns, item, 3);
    CompilerPatternDlg dlg(wxGetTopLevelParent(this), _("Update compiler error pattern"));
    dlg.SetPattern(pattern, lineIdx, fileIdx, colIdx);
    if(dlg.ShowModal() == wxID_OK) {
        SetColumnText(m_listErrPatterns, item, 0, dlg.GetPattern());
        SetColumnText(m_listErrPatterns, item, 1, dlg.GetFileIndex());
        SetColumnText(m_listErrPatterns, item, 2, dlg.GetLineIndex());
        SetColumnText(m_listErrPatterns, item, 3, dlg.GetColumnIndex());
    }
}

void CompilerMainPage::OnFileTypeActivated(wxListEvent& event)
{
    if(m_selectedFileType != wxNOT_FOUND) {
        EditCmpFileInfo dlg(wxGetTopLevelParent(this));
        dlg.SetCompilationLine(GetColumnText(m_listCtrlFileTypes, m_selectedFileType, 2));
        dlg.SetExtension(GetColumnText(m_listCtrlFileTypes, m_selectedFileType, 0));
        dlg.SetKind(GetColumnText(m_listCtrlFileTypes, m_selectedFileType, 1));

        if(dlg.ShowModal() == wxID_OK) {
            SetColumnText(m_listCtrlFileTypes, m_selectedFileType, 2, dlg.GetCompilationLine());
            SetColumnText(m_listCtrlFileTypes, m_selectedFileType, 0, dlg.GetExtension().Lower());
            SetColumnText(m_listCtrlFileTypes, m_selectedFileType, 1, dlg.GetKind());
        }
        m_isDirty = true;
    }
    event.Skip();
}

void CompilerMainPage::OnFileTypeDeSelected(wxListEvent& event)
{
#ifndef __WXMAC__
    m_selectedFileType = wxNOT_FOUND;
#endif
    event.Skip();
}

void CompilerMainPage::OnFileTypeSelected(wxListEvent& event)
{
    m_selectedFileType = event.m_itemIndex;
    event.Skip();
}

void CompilerMainPage::OnItemActivated(wxListEvent& event)
{
    m_isDirty = true;
    // get the var name & value
    wxListItem info;
    info.m_itemId = event.m_itemIndex;
    info.m_col = 0; // name
    info.m_mask = wxLIST_MASK_TEXT;

    if(m_listSwitches->GetItem(info)) {
        m_selSwitchName = info.m_text;
    }

    info.m_col = 1; // value
    if(m_listSwitches->GetItem(info)) {
        m_selSwitchValue = info.m_text;
    }
    EditSwitch();
}

void CompilerMainPage::OnItemSelected(wxListEvent& event)
{
    m_isDirty = true;
    // get the var name & value
    wxListItem info;
    info.m_itemId = event.m_itemIndex;
    info.m_col = 0; // name
    info.m_mask = wxLIST_MASK_TEXT;

    if(m_listSwitches->GetItem(info)) {
        m_selSwitchName = info.m_text;
    }

    info.m_col = 1; // value
    if(m_listSwitches->GetItem(info)) {
        m_selSwitchValue = info.m_text;
    }
}

void CompilerMainPage::OnLinkerOptionActivated(wxListEvent& event)
{
    if(m_selectedLnkOption == wxNOT_FOUND) {
        return;
    }

    wxString name = m_listLinkerOptions->GetItemText(m_selectedLnkOption);
    wxString help = GetColumnText(m_listLinkerOptions, m_selectedLnkOption, 1);
    CompilerLinkerOptionDialog dlg(wxGetTopLevelParent(this), name, help);
    if(dlg.ShowModal() == wxID_OK) {
        m_isDirty = true;
        SetColumnText(m_listLinkerOptions, m_selectedLnkOption, 0, dlg.GetName());
        SetColumnText(m_listLinkerOptions, m_selectedLnkOption, 1, dlg.GetHelp());
        m_listLinkerOptions->SetColumnWidth(1, wxLIST_AUTOSIZE);
    }
}

void CompilerMainPage::OnLinkerOptionDeSelected(wxListEvent& event)
{
#ifndef __WXMAC__
    m_selectedLnkOption = wxNOT_FOUND;
#endif
    event.Skip();
}

void CompilerMainPage::OnLinkerOptionSelected(wxListEvent& event)
{
    m_selectedLnkOption = event.m_itemIndex;
    event.Skip();
}

void CompilerMainPage::OnNewCompilerOption(wxCommandEvent& event)
{
    CompilerCompilerOptionDialog dlg(wxGetTopLevelParent(this), wxEmptyString, wxEmptyString);
    if(dlg.ShowModal() == wxID_OK) {
        long idx = m_listCompilerOptions->InsertItem(m_listCompilerOptions->GetItemCount(), dlg.GetName());
        m_listCompilerOptions->SetItem(idx, 1, dlg.GetHelp());
        m_listCompilerOptions->SetColumnWidth(1, wxLIST_AUTOSIZE);
        m_isDirty = true;
    }
}

void CompilerMainPage::OnNewFileType(wxCommandEvent& event)
{
    EditCmpFileInfo dlg(wxGetTopLevelParent(this));
    if(dlg.ShowModal() == wxID_OK) {
        long newItem = AppendListCtrlRow(m_listCtrlFileTypes);
        SetColumnText(m_listCtrlFileTypes, newItem, 2, dlg.GetCompilationLine());
        SetColumnText(m_listCtrlFileTypes, newItem, 0, dlg.GetExtension().Lower());
        SetColumnText(m_listCtrlFileTypes, newItem, 1, dlg.GetKind());
        m_isDirty = true;
    }
}

void CompilerMainPage::OnNewLinkerOption(wxCommandEvent& event)
{
    CompilerLinkerOptionDialog dlg(wxGetTopLevelParent(this), wxEmptyString, wxEmptyString);
    if(dlg.ShowModal() == wxID_OK) {
        m_isDirty = true;
        long idx = m_listLinkerOptions->InsertItem(m_listLinkerOptions->GetItemCount(), dlg.GetName());
        m_listLinkerOptions->SetItem(idx, 1, dlg.GetHelp());
        m_listLinkerOptions->SetColumnWidth(1, wxLIST_AUTOSIZE);
    }
}

void CompilerMainPage::OnWarnItemActivated(wxListEvent& event)
{
    DoUpdateWarnPattern(event.GetIndex());
    m_isDirty = true;
}

void CompilerMainPage::OnWarningPatternSelectedUI(wxUpdateUIEvent& event)
{
    int sel = m_listWarnPatterns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    event.Enable(sel != wxNOT_FOUND);
}

void CompilerMainPage::InitializePatterns()
{
    // Clear the content
    m_listErrPatterns->DeleteAllItems();
    m_listWarnPatterns->DeleteAllItems();

    // Populate with new content
    CHECK_PTR_RET(m_compiler);

    const Compiler::CmpListInfoPattern& errPatterns = m_compiler->GetErrPatterns();
    Compiler::CmpListInfoPattern::const_iterator itPattern;
    for(itPattern = errPatterns.begin(); itPattern != errPatterns.end(); ++itPattern) {
        long item = AppendListCtrlRow(m_listErrPatterns);
        SetColumnText(m_listErrPatterns, item, 0, itPattern->pattern);
        SetColumnText(m_listErrPatterns, item, 1, itPattern->fileNameIndex);
        SetColumnText(m_listErrPatterns, item, 2, itPattern->lineNumberIndex);
        SetColumnText(m_listErrPatterns, item, 3, itPattern->columnIndex);
    }

    const Compiler::CmpListInfoPattern& warnPatterns = m_compiler->GetWarnPatterns();
    for(itPattern = warnPatterns.begin(); itPattern != warnPatterns.end(); ++itPattern) {
        long item = AppendListCtrlRow(m_listWarnPatterns);
        SetColumnText(m_listWarnPatterns, item, 0, itPattern->pattern);
        SetColumnText(m_listWarnPatterns, item, 1, itPattern->fileNameIndex);
        SetColumnText(m_listWarnPatterns, item, 2, itPattern->lineNumberIndex);
        SetColumnText(m_listWarnPatterns, item, 3, itPattern->columnIndex);
    }
}

void CompilerMainPage::LoadCompiler(const wxString& compilerName)
{
    // Save before we switch
    if(m_isDirty) {
        Save();
    }
    m_compiler = BuildSettingsConfigST::Get()->GetCompiler(compilerName);

    // Start initialization
    Initialize();
}

void CompilerMainPage::SavePatterns()
{
    CHECK_PTR_RET(m_compiler);
    Compiler::CmpListInfoPattern errPatterns;
    for(int i = 0; i < m_listErrPatterns->GetItemCount(); ++i) {
        Compiler::CmpInfoPattern infoPattern;
        infoPattern.pattern = GetColumnText(m_listErrPatterns, i, 0);
        infoPattern.fileNameIndex = GetColumnText(m_listErrPatterns, i, 1);
        infoPattern.lineNumberIndex = GetColumnText(m_listErrPatterns, i, 2);
        infoPattern.columnIndex = GetColumnText(m_listErrPatterns, i, 3);
        errPatterns.push_back(infoPattern);
    }
    m_compiler->SetErrPatterns(errPatterns);

    Compiler::CmpListInfoPattern warnPatterns;
    for(int i = 0; i < m_listWarnPatterns->GetItemCount(); ++i) {
        Compiler::CmpInfoPattern infoPattern;
        infoPattern.pattern = GetColumnText(m_listWarnPatterns, i, 0);
        infoPattern.fileNameIndex = GetColumnText(m_listWarnPatterns, i, 1);
        infoPattern.lineNumberIndex = GetColumnText(m_listWarnPatterns, i, 2);
        infoPattern.columnIndex = GetColumnText(m_listWarnPatterns, i, 3);
        warnPatterns.push_back(infoPattern);
    }
    m_compiler->SetWarnPatterns(warnPatterns);
}

void CompilerMainPage::InitializeTools()
{
    // Clear the values
    wxPropertyGridConstIterator iter = m_pgMgrTools->GetGrid()->GetIterator();
    for(; !iter.AtEnd(); ++iter) {
        wxPGProperty* prop = iter.GetProperty();
        prop->SetValue("");
    }

    CHECK_PTR_RET(m_compiler);

    m_pgPropCXX->SetValue(m_compiler->GetTool(wxT("CXX")));
    m_pgPropCC->SetValue(m_compiler->GetTool(wxT("CC")));
    m_pgPropAR->SetValue(m_compiler->GetTool(wxT("AR")));
    m_pgPropLD->SetValue(m_compiler->GetTool("LinkerName"));
    m_pgPropSharedObjectLD->SetValue(m_compiler->GetTool("SharedObjectLinkerName"));
    m_pgPropAS->SetValue(m_compiler->GetTool("AS"));
    m_pgPropMAKE->SetValue(m_compiler->GetTool("MAKE"));
    m_pgPropResourceCompiler->SetValue(m_compiler->GetTool("ResourceCompiler"));
    m_pgPropMkdir->SetValue(m_compiler->GetTool("MakeDirCommand"));
    m_pgPropDebugger->SetValue(m_compiler->GetTool("Debugger"));
}

void CompilerMainPage::SaveTools()
{
    CHECK_PTR_RET(m_compiler);
    m_compiler->SetTool(wxT("CXX"), m_pgPropCXX->GetValueAsString());
    m_compiler->SetTool(wxT("CC"), m_pgPropCC->GetValueAsString());
    m_compiler->SetTool(wxT("AR"), m_pgPropAR->GetValueAsString());
    m_compiler->SetTool(wxT("LinkerName"), m_pgPropLD->GetValueAsString());
    m_compiler->SetTool(wxT("SharedObjectLinkerName"), m_pgPropSharedObjectLD->GetValueAsString());
    m_compiler->SetTool(wxT("ResourceCompiler"), m_pgPropResourceCompiler->GetValueAsString());
    m_compiler->SetTool("MAKE", m_pgPropMAKE->GetValueAsString());
    m_compiler->SetTool("AS", m_pgPropAS->GetValueAsString());
    m_compiler->SetTool("MakeDirCommand", m_pgPropMkdir->GetValueAsString());
    m_compiler->SetTool("Debugger", m_pgPropDebugger->GetValueAsString());
}

void CompilerMainPage::Initialize()
{
    InitializeTools();
    InitializePatterns();
    InitializeSwitches();
    InitializeFileTypes();
    InitializeAdvancePage();
    InitializeCompilerOptions();
    InitializeLinkerOptions();
}

void CompilerMainPage::Save()
{
    SaveTools();
    SavePatterns();
    SaveSwitches();
    SaveFileTypes();
    SaveAdvancedPage();
    SaveComilerOptions();
    SaveLinkerOptions();

    // save the compiler to the file system
    BuildSettingsConfigST::Get()->SetCompiler(m_compiler);
    m_isDirty = false;
}

void CompilerMainPage::AddSwitch(const wxString& name, const wxString& value, bool choose)
{
    long item = AppendListCtrlRow(m_listSwitches);
    SetColumnText(m_listSwitches, item, 0, name);
    SetColumnText(m_listSwitches, item, 1, value);

    if(choose) {
        m_selSwitchName = name;
        m_selSwitchValue = value;
    }
    m_listSwitches->SetColumnWidth(0, wxLIST_AUTOSIZE);
    m_listSwitches->SetColumnWidth(0, 100);
}

void CompilerMainPage::EditSwitch()
{
    wxString message;
    message << m_selSwitchName << _(" switch:");
    wxTextEntryDialog dlg(this, message, _("Edit"), m_selSwitchValue);
    if(dlg.ShowModal() == wxID_OK) {
        wxString newVal = dlg.GetValue();
        m_compiler->SetSwitch(m_selSwitchName, dlg.GetValue());
        m_isDirty = true;
        InitializeSwitches();
    }
}

void CompilerMainPage::InitializeSwitches()
{
    // Clear content
    wxWindowUpdateLocker locker(m_listSwitches);
    m_listSwitches->DeleteAllItems();
    m_selSwitchName.Clear();
    m_selSwitchValue.Clear();

    // Validate compiler and add new content
    CHECK_PTR_RET(m_compiler);

    Compiler::ConstIterator iter = m_compiler->SwitchesBegin();
    for(; iter != m_compiler->SwitchesEnd(); iter++) {
        AddSwitch(iter->first, iter->second, iter == m_compiler->SwitchesBegin());
    }
    m_listSwitches->SetColumnWidth(0, wxLIST_AUTOSIZE);
}

void CompilerMainPage::SaveSwitches() {}

void CompilerMainPage::InitializeFileTypes()
{
    // populate the list control
    wxWindowUpdateLocker locker(m_listCtrlFileTypes);

    m_listCtrlFileTypes->DeleteAllItems();
    CHECK_PTR_RET(m_compiler);

    m_selectedFileType = -1;
    std::map<wxString, Compiler::CmpFileTypeInfo> fileTypes = m_compiler->GetFileTypes();
    std::map<wxString, Compiler::CmpFileTypeInfo>::iterator iter = fileTypes.begin();
    for(; iter != fileTypes.end(); iter++) {
        Compiler::CmpFileTypeInfo ft = iter->second;

        long item = AppendListCtrlRow(m_listCtrlFileTypes);
        SetColumnText(m_listCtrlFileTypes, item, 0, ft.extension);
        SetColumnText(
            m_listCtrlFileTypes, item, 1, ft.kind == Compiler::CmpFileKindSource ? _("Source") : _("Resource"));
        SetColumnText(m_listCtrlFileTypes, item, 2, ft.compilation_line);
    }

    m_listCtrlFileTypes->SetColumnWidth(0, 70);
    m_listCtrlFileTypes->SetColumnWidth(1, 70);
    m_listCtrlFileTypes->SetColumnWidth(2, wxLIST_AUTOSIZE);
}

void CompilerMainPage::SaveFileTypes()
{
    CHECK_PTR_RET(m_compiler);

    std::map<wxString, Compiler::CmpFileTypeInfo> fileTypes;
    int count = m_listCtrlFileTypes->GetItemCount();
    for(int i = 0; i < count; i++) {
        Compiler::CmpFileTypeInfo ft;
        ft.extension = GetColumnText(m_listCtrlFileTypes, i, 0);
        ft.kind = GetColumnText(m_listCtrlFileTypes, i, 1) == _("Resource") ? Compiler::CmpFileKindResource :
                                                                              Compiler::CmpFileKindSource;
        ft.compilation_line = GetColumnText(m_listCtrlFileTypes, i, 2);

        fileTypes[ft.extension] = ft;
    }
    m_compiler->SetFileTypes(fileTypes);
}

void CompilerMainPage::InitializeAdvancePage()
{
    // Clear old conetnt
    m_textObjectExtension->ChangeValue("");
    m_textDependExtension->ChangeValue("");
    m_textPreprocessExtension->ChangeValue("");
    m_checkBoxGenerateDependenciesFiles->SetValue(false);
    m_textCtrlGlobalIncludePath->ChangeValue("");
    m_textCtrlGlobalLibPath->ChangeValue("");
    m_checkBoxReadObjectsFromFile->SetValue(false);
    m_checkBoxObjectNameSameAsFileName->SetValue(false);

    CHECK_PTR_RET(m_compiler);
    m_textObjectExtension->ChangeValue(m_compiler->GetObjectSuffix());
    m_textDependExtension->ChangeValue(m_compiler->GetDependSuffix());
    m_textPreprocessExtension->ChangeValue(m_compiler->GetPreprocessSuffix());
    m_checkBoxGenerateDependenciesFiles->SetValue(m_compiler->GetGenerateDependeciesFile());
    m_textCtrlGlobalIncludePath->ChangeValue(m_compiler->GetGlobalIncludePath());
    m_textCtrlGlobalLibPath->ChangeValue(m_compiler->GetGlobalLibPath());
    m_checkBoxReadObjectsFromFile->SetValue(m_compiler->GetReadObjectFilesFromList());
    m_checkBoxObjectNameSameAsFileName->SetValue(m_compiler->GetObjectNameIdenticalToFileName());
}

void CompilerMainPage::SaveAdvancedPage()
{
    CHECK_PTR_RET(m_compiler);
    m_compiler->SetGenerateDependeciesFile(m_checkBoxGenerateDependenciesFiles->IsChecked());
    m_compiler->SetGlobalIncludePath(m_textCtrlGlobalIncludePath->GetValue());
    m_compiler->SetGlobalLibPath(m_textCtrlGlobalLibPath->GetValue());
    m_compiler->SetObjectSuffix(m_textObjectExtension->GetValue());
    m_compiler->SetDependSuffix(m_textDependExtension->GetValue());
    m_compiler->SetPreprocessSuffix(m_textPreprocessExtension->GetValue());
    m_compiler->SetReadObjectFilesFromList(m_checkBoxReadObjectsFromFile->IsChecked());
    m_compiler->SetObjectNameIdenticalToFileName(m_checkBoxObjectNameSameAsFileName->IsChecked());
}

void CompilerMainPage::InitializeCompilerOptions()
{
    m_selectedCmpOption = -1;
    wxWindowUpdateLocker locker(m_listCompilerOptions);
    m_listCompilerOptions->DeleteAllItems();

    CHECK_PTR_RET(m_compiler);
    const Compiler::CmpCmdLineOptions& cmpOptions = m_compiler->GetCompilerOptions();
    Compiler::CmpCmdLineOptions::const_iterator itCmpOption = cmpOptions.begin();
    for(; itCmpOption != cmpOptions.end(); ++itCmpOption) {
        const Compiler::CmpCmdLineOption& cmpOption = itCmpOption->second;
        long idx = m_listCompilerOptions->InsertItem(m_listCompilerOptions->GetItemCount(), cmpOption.name);
        m_listCompilerOptions->SetItem(idx, 1, cmpOption.help);
    }

    m_listCompilerOptions->SetColumnWidth(0, 100);
    m_listCompilerOptions->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void CompilerMainPage::SaveComilerOptions()
{
    CHECK_PTR_RET(m_compiler);
    Compiler::CmpCmdLineOptions cmpOptions;
    for(int idx = 0; idx < m_listCompilerOptions->GetItemCount(); ++idx) {
        Compiler::CmpCmdLineOption cmpOption;
        cmpOption.name = m_listCompilerOptions->GetItemText(idx);
        cmpOption.help = GetColumnText(m_listCompilerOptions, idx, 1);

        cmpOptions[cmpOption.name] = cmpOption;
    }
    m_compiler->SetCompilerOptions(cmpOptions);
}

void CompilerMainPage::InitializeLinkerOptions()
{
    m_selectedLnkOption = -1;
    wxWindowUpdateLocker locker(m_listLinkerOptions);
    m_listLinkerOptions->DeleteAllItems();
    CHECK_PTR_RET(m_compiler);

    const Compiler::CmpCmdLineOptions& lnkOptions = m_compiler->GetLinkerOptions();
    Compiler::CmpCmdLineOptions::const_iterator itLnkOption = lnkOptions.begin();
    for(; itLnkOption != lnkOptions.end(); ++itLnkOption) {
        const Compiler::CmpCmdLineOption& lnkOption = itLnkOption->second;
        long idx = m_listLinkerOptions->InsertItem(m_listLinkerOptions->GetItemCount(), lnkOption.name);
        m_listLinkerOptions->SetItem(idx, 1, lnkOption.help);
    }
    m_listLinkerOptions->SetColumnWidth(0, 100);
    m_listLinkerOptions->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void CompilerMainPage::SaveLinkerOptions()
{
    CHECK_PTR_RET(m_compiler);
    Compiler::CmpCmdLineOptions lnkOptions;
    for(int idx = 0; idx < m_listLinkerOptions->GetItemCount(); ++idx) {
        Compiler::CmpCmdLineOption lnkOption;
        lnkOption.name = m_listLinkerOptions->GetItemText(idx);
        lnkOption.help = GetColumnText(m_listLinkerOptions, idx, 1);

        lnkOptions[lnkOption.name] = lnkOption;
    }
    m_compiler->SetLinkerOptions(lnkOptions);
}

void CompilerMainPage::LoadCompilers()
{
    // Populate the compilers list
    m_listBoxCompilers->Clear();

    wxString cmpType;
    if(clCxxWorkspaceST::Get()->IsOpen() && clCxxWorkspaceST::Get()->GetActiveProject()) {
        BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetActiveProject()->GetBuildConfiguration();
        if(bldConf) {
            cmpType = bldConf->GetCompilerType();
        }
    }

    BuildSettingsConfigCookie cookie;
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
    int sel(0);
    while(cmp) {
        int curidx = m_listBoxCompilers->Append(cmp->GetName());
        if(!cmpType.IsEmpty() && (cmp->GetName() == cmpType)) {
            sel = curidx;
        }
        cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
    }

    if(!m_listBoxCompilers->IsEmpty()) {
        m_listBoxCompilers->SetSelection(sel);
        LoadCompiler(m_listBoxCompilers->GetStringSelection());
    }
}

void CompilerMainPage::OnCompilerSelected(wxCommandEvent& event) { LoadCompiler(event.GetString()); }

// ======----------------------------------------------------------------
// Helper dialogs
// ======----------------------------------------------------------------

CompilerPatternDlg::CompilerPatternDlg(wxWindow* parent, const wxString& title)
    : CompilerPatternDlgBase(parent, wxID_ANY, title)
{
    SetName("CompilerPatternDlg");
    WindowAttrManager::Load(this);
}

CompilerPatternDlg::~CompilerPatternDlg() {  }

void CompilerPatternDlg::SetPattern(const wxString& pattern,
                                    const wxString& lineIdx,
                                    const wxString& fileIdx,
                                    const wxString& columnIndex)
{
    m_textPattern->ChangeValue(pattern);
    m_textLineNumber->ChangeValue(lineIdx);
    m_textFileIndex->ChangeValue(fileIdx);
    m_textColumn->ChangeValue(columnIndex);
}

void CompilerPatternDlg::OnSubmit(wxCommandEvent& event)
{
    if(GetPattern().Trim().IsEmpty() || GetFileIndex().Trim().IsEmpty() || GetLineIndex().Trim().IsEmpty()) {
        wxMessageBox(_("Please fill all the fields"), _("CodeLite"), wxOK | wxICON_INFORMATION, this);
        return;
    }
    EndModal(wxID_OK);
}

void CompilerMainPage::OnContextMenu(wxContextMenuEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("rename_compiler"), _("Rename..."));
    menu.Append(wxID_DELETE);

    int selection = m_listBoxCompilers->GetSelection();
    menu.Enable(wxID_DELETE, selection != wxNOT_FOUND);
    menu.Enable(XRCID("rename_compiler"), selection != wxNOT_FOUND);

    m_listBoxCompilers->Bind(
        wxEVT_COMMAND_MENU_SELECTED, &CompilerMainPage::OnRenameCompiler, this, XRCID("rename_compiler"));
    m_listBoxCompilers->Bind(wxEVT_COMMAND_MENU_SELECTED, &CompilerMainPage::OnDeleteCompiler, this, wxID_DELETE);

    m_listBoxCompilers->PopupMenu(&menu);
}

void CompilerMainPage::OnDeleteCompiler(wxCommandEvent& event)
{
    int selection = m_listBoxCompilers->GetSelection();
    if(selection == wxNOT_FOUND) return;

    if(::wxMessageBox(wxString() << _("Are you sure you want to delete compiler\n'")
                                 << m_listBoxCompilers->GetStringSelection() << "'?",
                      _("Delete Compiler"),
                      wxYES_NO | wxCENTER | wxICON_WARNING) != wxYES)
        return;

    wxString compilerName = m_listBoxCompilers->GetStringSelection();
    BuildSettingsConfigST::Get()->DeleteCompiler(compilerName);

    // Reload the content
    LoadCompilers();
}

void CompilerMainPage::OnRenameCompiler(wxCommandEvent& event)
{
    int selection = m_listBoxCompilers->GetSelection();
    if(selection == wxNOT_FOUND) return;

    wxString newName =
        ::wxGetTextFromUser(_("New Compiler Name"), _("Rename Compiler"), m_listBoxCompilers->GetStringSelection());
    if(newName.IsEmpty()) return;

    CompilerPtr compiler = BuildSettingsConfigST::Get()->GetCompiler(m_listBoxCompilers->GetStringSelection());
    if(!compiler) return;

    // Delete the old compiler
    BuildSettingsConfigST::Get()->DeleteCompiler(compiler->GetName());

    // Create new one with differet name
    compiler->SetName(newName);
    BuildSettingsConfigST::Get()->SetCompiler(compiler);

    // Reload the content
    LoadCompilers();
}
void CompilerMainPage::OnValueChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    m_isDirty = true;
}

void CompilerMainPage::OnCmdModify(wxCommandEvent& event)
{
    event.Skip();
    m_isDirty = true;
}
void CompilerMainPage::OnAddExistingCompiler(wxCommandEvent& event)
{
    BuildSettingsDialog* dlg = dynamic_cast<BuildSettingsDialog*>(wxGetTopLevelParent(this));
    dlg->CallAfter(&BuildSettingsDialog::OnAddExistingCompiler);
}

void CompilerMainPage::OnCloneCompiler(wxCommandEvent& event)
{
    BuildSettingsDialog* dlg = dynamic_cast<BuildSettingsDialog*>(wxGetTopLevelParent(this));
    dlg->CallAfter(&BuildSettingsDialog::OnButtonNewClicked);
}

void CompilerMainPage::OnScanCompilers(wxCommandEvent& event)
{
    BuildSettingsDialog* dlg = dynamic_cast<BuildSettingsDialog*>(wxGetTopLevelParent(this));
    dlg->CallAfter(&BuildSettingsDialog::OnScanAndSuggestCompilers);
}
