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

#include "EditCmpTemplateDialog.h"
#include "EditDlg.h"
#include "NewFileTemplateDialog.h"
#include "advanced_settings.h"
#include "build_config.h"
#include "build_settings_config.h"
#include "compiler.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "windowattrmanager.h"
#include "workspace.h"

#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/wupdlock.h>

CompilerMainPage::CompilerMainPage(wxWindow* parent)
    : CompilerMainPageBase(parent)
    , m_isDirty(false)
    , m_compiler(NULL)
    , m_selectedCmpOption(-1)
    , m_selectedLnkOption(-1)
{
    // =============-----------------------------
    // Patterns page initialization
    // =============-----------------------------
    m_dvListCtrlErrors->AddHeader(_("Pattern"));
    m_dvListCtrlErrors->AddHeader(_("File name index"), wxNullBitmap, wxCOL_WIDTH_DEFAULT);
    m_dvListCtrlErrors->AddHeader(_("Line number index"), wxNullBitmap, wxCOL_WIDTH_DEFAULT);
    m_dvListCtrlErrors->AddHeader(_("Column index"), wxNullBitmap, wxCOL_WIDTH_DEFAULT);

    m_dvListCtrlWarnings->AddHeader(_("Pattern"));
    m_dvListCtrlWarnings->AddHeader(_("File name index"), wxNullBitmap, wxCOL_WIDTH_DEFAULT);
    m_dvListCtrlWarnings->AddHeader(_("Line number index"), wxNullBitmap, wxCOL_WIDTH_DEFAULT);
    m_dvListCtrlWarnings->AddHeader(_("Column index"), wxNullBitmap, wxCOL_WIDTH_DEFAULT);

    // ==============------------------
    // Switches
    // ==============------------------
    m_listSwitches->InsertColumn(0, _("Name"));
    m_listSwitches->InsertColumn(1, _("Value"));

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

    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(wxID_NEW, _("Add an existing compiler"), images->Add("file_new"));
    m_toolbar->AddTool(wxID_COPY, _("Copy compiler"), images->Add("copy"));
    m_toolbar->AddTool(wxID_FIND, _("Scan for compilers"), images->Add("find"));
    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL, &CompilerMainPage::OnAddExistingCompiler, this, wxID_NEW);
    m_toolbar->Bind(wxEVT_TOOL, &CompilerMainPage::OnCloneCompiler, this, wxID_COPY);
    m_toolbar->Bind(wxEVT_TOOL, &CompilerMainPage::OnScanCompilers, this, wxID_FIND);
}

CompilerMainPage::~CompilerMainPage() {}

void CompilerMainPage::OnBtnAddErrPattern(wxCommandEvent& event)
{
    CompilerPatternDlg dlg(wxGetTopLevelParent(this), _("Add compiler error pattern"));
    if(dlg.ShowModal() == wxID_OK) {
        auto item = m_dvListCtrlErrors->AppendItem(dlg.GetPattern());
        m_dvListCtrlErrors->SetItemText(item, dlg.GetFileIndex(), 1);
        m_dvListCtrlErrors->SetItemText(item, dlg.GetLineIndex(), 2);
        m_dvListCtrlErrors->SetItemText(item, dlg.GetColumnIndex(), 3);
        m_isDirty = true;
    }
}

void CompilerMainPage::OnBtnAddWarnPattern(wxCommandEvent& event)
{
    CompilerPatternDlg dlg(wxGetTopLevelParent(this), _("Add compiler warning pattern"));
    if(dlg.ShowModal() == wxID_OK) {
        auto item = m_dvListCtrlWarnings->AppendItem(dlg.GetPattern());
        m_dvListCtrlWarnings->SetItemText(item, dlg.GetFileIndex(), 1);
        m_dvListCtrlWarnings->SetItemText(item, dlg.GetLineIndex(), 2);
        m_dvListCtrlWarnings->SetItemText(item, dlg.GetColumnIndex(), 3);
        m_isDirty = true;
    }
}

void CompilerMainPage::OnBtnDelErrPattern(wxCommandEvent& event)
{
    wxUnusedVar(event);
    auto item = m_dvListCtrlErrors->GetSelection();
    CHECK_ITEM_RET(item);

    m_dvListCtrlErrors->DeleteItem(m_dvListCtrlErrors->ItemToRow(item));
    m_isDirty = true;
}

void CompilerMainPage::OnBtnDelWarnPattern(wxCommandEvent& event)
{
    wxUnusedVar(event);
    auto item = m_dvListCtrlWarnings->GetSelection();
    CHECK_ITEM_RET(item);

    m_dvListCtrlWarnings->DeleteItem(m_dvListCtrlWarnings->ItemToRow(item));
    m_isDirty = true;
}

void CompilerMainPage::OnBtnUpdateErrPattern(wxCommandEvent& event)
{
    wxUnusedVar(event);
    auto item = m_dvListCtrlErrors->GetSelection();
    CHECK_ITEM_RET(item);

    DoUpdateErrPattern(item);
    m_isDirty = true;
}

void CompilerMainPage::OnBtnUpdateWarnPattern(wxCommandEvent& event)
{
    wxUnusedVar(event);
    auto item = m_dvListCtrlWarnings->GetSelection();
    CHECK_ITEM_RET(item);

    DoUpdateWarnPattern(item);
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
        if(wxMessageBox(_("Are you sure you want to delete this compiler option?"), _("CodeLite"),
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
    if(m_dvListCtrlFileTemplates->GetSelectedItemsCount() == 0) {
        return;
    }
    int sel = m_dvListCtrlFileTemplates->GetSelectedRow();
    if(sel == wxNOT_FOUND) {
        return;
    }
    wxDataViewItem item = m_dvListCtrlFileTemplates->RowToItem(sel);
    wxString filetype = m_dvListCtrlFileTemplates->GetItemText(item);

    if(wxMessageBox(wxString() << _("Are you sure you want to delete '") << filetype << "'?", _("CodeLite"),
                    wxYES_NO | wxCANCEL) == wxYES) {
        m_dvListCtrlFileTemplates->DeleteItem(sel);
        m_isDirty = true;
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

void CompilerMainPage::OnErrItemActivated(wxDataViewEvent& event)
{
    DoUpdateErrPattern(event.GetItem());
    m_isDirty = true;
}

void CompilerMainPage::OnErrorPatternSelectedUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_dvListCtrlErrors->IsEmpty());
}

void CompilerMainPage::DoUpdatePattern(clThemedListCtrl* list, const wxDataViewItem& item, const wxString& dialog_title)
{
    wxString pattern = list->GetItemText(item, 0);
    wxString fileIdx = list->GetItemText(item, 1);
    wxString lineIdx = list->GetItemText(item, 2);
    wxString colIdx = list->GetItemText(item, 3);

    CompilerPatternDlg dlg(wxGetTopLevelParent(this), dialog_title);
    dlg.SetPattern(pattern, lineIdx, fileIdx, colIdx);
    if(dlg.ShowModal() != wxID_OK) {
        return;
    }

    list->SetItemText(item, dlg.GetPattern(), 0);
    list->SetItemText(item, dlg.GetFileIndex(), 1);
    list->SetItemText(item, dlg.GetLineIndex(), 2);
    list->SetItemText(item, dlg.GetColumnIndex(), 3);
}

void CompilerMainPage::DoUpdateWarnPattern(const wxDataViewItem& item)
{
    DoUpdatePattern(m_dvListCtrlWarnings, item, _("Update compiler warning pattern"));
}

void CompilerMainPage::DoUpdateErrPattern(const wxDataViewItem& item)
{
    DoUpdatePattern(m_dvListCtrlErrors, item, _("Update compiler error pattern"));
}

void CompilerMainPage::DoFileTypeActivated(const wxDataViewItem& item)
{
    if(!item.IsOk()) {
        return;
    }

    NewFileTemplateDialog dlg(wxGetTopLevelParent(this));
    wxString ext = m_dvListCtrlFileTemplates->GetItemText(item, 0);
    wxString kind = m_dvListCtrlFileTemplates->GetItemText(item, 1);
    wxString pattern = m_dvListCtrlFileTemplates->GetItemText(item, 2);

    dlg.SetPattern(pattern);
    dlg.SetExtension(ext);
    dlg.SetKind(kind);

    if(dlg.ShowModal() == wxID_OK) {
        m_dvListCtrlFileTemplates->SetItemText(item, dlg.GetExtension().Lower(), 0);
        m_dvListCtrlFileTemplates->SetItemText(item, dlg.GetKind(), 1);
        m_dvListCtrlFileTemplates->SetItemText(item, dlg.GetPattern(), 2);
        m_isDirty = true;
    }
}

void CompilerMainPage::OnFileTypeActivated(wxDataViewEvent& event) { DoFileTypeActivated(event.GetItem()); }

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
    NewFileTemplateDialog dlg(wxGetTopLevelParent(this));
    if(dlg.ShowModal() == wxID_OK) {
        wxDataViewItem item = m_dvListCtrlFileTemplates->AppendItem(dlg.GetExtension().Lower());
        m_dvListCtrlFileTemplates->SetItemText(item, dlg.GetKind(), 1);
        m_dvListCtrlFileTemplates->SetItemText(item, dlg.GetPattern(), 2);
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

void CompilerMainPage::OnWarnItemActivated(wxDataViewEvent& event)
{
    DoUpdateWarnPattern(event.GetItem());
    m_isDirty = true;
}

void CompilerMainPage::OnWarningPatternSelectedUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_dvListCtrlWarnings->IsEmpty());
}

void CompilerMainPage::DoAddPattern(clThemedListCtrl* list, const Compiler::CmpInfoPattern& pattern)
{
    auto item = list->AppendItem(pattern.pattern);
    list->SetItemText(item, pattern.fileNameIndex, 1);
    list->SetItemText(item, pattern.lineNumberIndex, 2);
    list->SetItemText(item, pattern.columnIndex, 3);
}

void CompilerMainPage::InitializePatterns()
{
    // Clear the content
    m_dvListCtrlErrors->DeleteAllItems();
    m_dvListCtrlWarnings->DeleteAllItems();

    // Populate with new content
    CHECK_PTR_RET(m_compiler);

    for(const auto& pattern : m_compiler->GetErrPatterns()) {
        DoAddPattern(m_dvListCtrlErrors, pattern);
    }

    for(const auto& pattern : m_compiler->GetWarnPatterns()) {
        DoAddPattern(m_dvListCtrlWarnings, pattern);
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

    for(int i = 0; i < m_dvListCtrlErrors->GetItemCount(); ++i) {
        auto item = m_dvListCtrlErrors->RowToItem(i);
        Compiler::CmpInfoPattern infoPattern;
        infoPattern.pattern = m_dvListCtrlErrors->GetItemText(item, 0);
        infoPattern.fileNameIndex = m_dvListCtrlErrors->GetItemText(item, 1);
        infoPattern.lineNumberIndex = m_dvListCtrlErrors->GetItemText(item, 2);
        infoPattern.columnIndex = m_dvListCtrlErrors->GetItemText(item, 3);
        errPatterns.push_back(infoPattern);
    }
    m_compiler->SetErrPatterns(errPatterns);

    Compiler::CmpListInfoPattern warnPatterns;
    for(int i = 0; i < m_dvListCtrlWarnings->GetItemCount(); ++i) {
        auto item = m_dvListCtrlWarnings->RowToItem(i);
        Compiler::CmpInfoPattern infoPattern;
        infoPattern.pattern = m_dvListCtrlWarnings->GetItemText(item, 0);
        infoPattern.fileNameIndex = m_dvListCtrlWarnings->GetItemText(item, 1);
        infoPattern.lineNumberIndex = m_dvListCtrlWarnings->GetItemText(item, 2);
        infoPattern.columnIndex = m_dvListCtrlWarnings->GetItemText(item, 3);
        warnPatterns.push_back(infoPattern);
    }
    m_compiler->SetWarnPatterns(warnPatterns);
}

void CompilerMainPage::InitializeTools()
{
    // Clear the values
    const wxPropertyGrid* pgrid = m_pgMgrTools->GetGrid();
    wxPropertyGridConstIterator iter = pgrid->GetIterator();
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
    InitialiseTemplates();
    InitializeAdvancePage();
    InitializeCompilerOptions();
    InitializeLinkerOptions();
}

void CompilerMainPage::Save()
{
    CHECK_PTR_RET(m_compiler);
    SaveTools();
    SavePatterns();
    SaveSwitches();
    SaveTemplates();
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

void CompilerMainPage::InitialiseTemplates()
{
    m_dvListCtrlFileTemplates->DeleteAllItems();
    m_dvListCtrlLinkType->DeleteAllItems();

    CHECK_PTR_RET(m_compiler);
    m_checkBoxReadObjectsFromFile->SetValue(m_compiler->GetReadObjectFilesFromList());

    const auto& fileTypes = m_compiler->GetFileTypes();
    wxVector<wxVariant> cols;
    for(const auto& vt : fileTypes) {
        const Compiler::CmpFileTypeInfo& ft = vt.second;
        cols.clear();
        cols.push_back(ft.extension);
        cols.push_back(ft.kind == Compiler::CmpFileKindResource ? "Resource" : "Source");
        cols.push_back(ft.compilation_line);
        m_dvListCtrlFileTemplates->AppendItem(cols);
    }

    bool useFile = m_checkBoxReadObjectsFromFile->IsChecked();
    const auto& linkerLines = m_compiler->GetLinkerLines();
    for(const auto& vt : linkerLines) {
        const auto& type = vt.first;
        const auto& line = useFile ? vt.second.lineFromFile : vt.second.line;
        cols.clear();
        cols.push_back(type);
        cols.push_back(line);
        m_dvListCtrlLinkType->AppendItem(cols);
    }
}

void CompilerMainPage::SaveTemplates()
{
    CHECK_PTR_RET(m_compiler);

    std::map<wxString, Compiler::CmpFileTypeInfo> fileTypes;
    size_t count = m_dvListCtrlFileTemplates->GetItemCount();
    for(size_t i = 0; i < count; ++i) {
        Compiler::CmpFileTypeInfo ft;
        wxDataViewItem item = m_dvListCtrlFileTemplates->RowToItem(i);
        ft.extension = m_dvListCtrlFileTemplates->GetItemText(item, 0);
        ft.kind = (m_dvListCtrlFileTemplates->GetItemText(item, 1) == "Resource") ? Compiler::CmpFileKindResource
                                                                                  : Compiler::CmpFileKindSource;
        ft.compilation_line = m_dvListCtrlFileTemplates->GetItemText(item, 2);
        fileTypes[ft.extension] = ft;
    }
    m_compiler->SetFileTypes(fileTypes);

    count = m_dvListCtrlLinkType->GetItemCount();
    for(size_t i = 0; i < count; ++i) {
        wxDataViewItem item = m_dvListCtrlLinkType->RowToItem(i);
        wxString type = m_dvListCtrlLinkType->GetItemText(item, 0);
        wxString pattern = m_dvListCtrlLinkType->GetItemText(item, 1);
        m_compiler->SetLinkLine(type, pattern, m_checkBoxReadObjectsFromFile->IsChecked());
    }
}

void CompilerMainPage::InitializeAdvancePage()
{
    // Clear old conetnt
    m_textObjectExtension->ChangeValue("");
    m_textDependExtension->ChangeValue("");
    m_textPreprocessExtension->ChangeValue("");
    m_checkBoxGenerateDependenciesFiles->Enable(false);
    m_checkBoxGenerateDependenciesFiles->SetValue(false);
    m_textCtrlGlobalIncludePath->ChangeValue("");
    m_textCtrlGlobalLibPath->ChangeValue("");
    m_checkBoxObjectNameSameAsFileName->SetValue(false);

    CHECK_PTR_RET(m_compiler);
    m_textObjectExtension->ChangeValue(m_compiler->GetObjectSuffix());
    m_textDependExtension->ChangeValue(m_compiler->GetDependSuffix());
    m_textPreprocessExtension->ChangeValue(m_compiler->GetPreprocessSuffix());
    m_checkBoxGenerateDependenciesFiles->Enable(m_compiler->IsGnuCompatibleCompiler());
    m_checkBoxGenerateDependenciesFiles->SetValue(m_compiler->GetGenerateDependeciesFile());
    m_textCtrlGlobalIncludePath->ChangeValue(m_compiler->GetGlobalIncludePath());
    m_textCtrlGlobalLibPath->ChangeValue(m_compiler->GetGlobalLibPath());
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
    for (const auto& [_, cmpOption] : m_compiler->GetCompilerOptions()) {
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

    for (const auto& [_, lnkOption] : m_compiler->GetLinkerOptions()) {
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

    if(parent) {
        wxSize parentSize = parent->GetSize();
        double dlgWidth = (double)parentSize.GetWidth() * 0.67;
        parentSize.SetWidth(dlgWidth);
        parentSize.SetHeight(wxNOT_FOUND);
        SetSize(parentSize);
        SetSizeHints(parentSize);
        GetSizer()->Fit(this);
        GetSizer()->Layout();
        CentreOnParent();
    }
}

CompilerPatternDlg::~CompilerPatternDlg() {}

void CompilerPatternDlg::SetPattern(const wxString& pattern, const wxString& lineIdx, const wxString& fileIdx,
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

    m_listBoxCompilers->Bind(wxEVT_COMMAND_MENU_SELECTED, &CompilerMainPage::OnRenameCompiler, this,
                             XRCID("rename_compiler"));
    m_listBoxCompilers->Bind(wxEVT_COMMAND_MENU_SELECTED, &CompilerMainPage::OnDeleteCompiler, this, wxID_DELETE);

    m_listBoxCompilers->PopupMenu(&menu);
}

void CompilerMainPage::OnDeleteCompiler(wxCommandEvent& event)
{
    int selection = m_listBoxCompilers->GetSelection();
    if(selection == wxNOT_FOUND)
        return;

    if(::wxMessageBox(wxString() << _("Are you sure you want to delete compiler\n'")
                                 << m_listBoxCompilers->GetStringSelection() << "'?",
                      _("Delete Compiler"), wxYES_NO | wxCENTER | wxICON_WARNING) != wxYES)
        return;

    wxString compilerName = m_listBoxCompilers->GetStringSelection();
    BuildSettingsConfigST::Get()->DeleteCompiler(compilerName);

    // Reload the content
    LoadCompilers();
}

void CompilerMainPage::OnRenameCompiler(wxCommandEvent& event)
{
    int selection = m_listBoxCompilers->GetSelection();
    if(selection == wxNOT_FOUND)
        return;

    wxString newName =
        ::wxGetTextFromUser(_("New Compiler Name"), _("Rename Compiler"), m_listBoxCompilers->GetStringSelection());
    if(newName.IsEmpty())
        return;

    CompilerPtr compiler = BuildSettingsConfigST::Get()->GetCompiler(m_listBoxCompilers->GetStringSelection());
    if(!compiler)
        return;

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
void CompilerMainPage::OnLinkerUseFileInput(wxCommandEvent& event)
{
    CHECK_PTR_RET(m_compiler);
    m_compiler->SetReadObjectFilesFromList(event.IsChecked());
    InitialiseTemplates();
    m_isDirty = true;
}
void CompilerMainPage::OnLinkLineActivated(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();
    CHECK_ITEM_RET(item);

    EditCmpTemplateDialog dlg(wxGetTopLevelParent(this));
    dlg.SetPattern(m_dvListCtrlLinkType->GetItemText(item, 1));
    if(dlg.ShowModal() == wxID_OK) {
        m_dvListCtrlLinkType->SetItemText(item, dlg.GetPattern(), 1);
        m_isDirty = true;
    }
}
