//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : open_resource_dialog.cpp
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

#include "open_resource_dialog.h"
#include "bitmap_loader.h"
#include <wx/imaglist.h>
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include "window_locker.h"
#include "editor_config.h"
#include "ieditor.h"
#include "ctags_manager.h"
#include <wx/filefn.h>
#include "project.h"
#include "workspace.h"
#include <wx/wupdlock.h>
#include "imanager.h"
#include "windowattrmanager.h"
#include <vector>
#include <codelite_events.h>
#include "event_notifier.h"

BEGIN_EVENT_TABLE(OpenResourceDialog, OpenResourceDialogBase)
EVT_TIMER(XRCID("OR_TIMER"), OpenResourceDialog::OnTimer)
END_EVENT_TABLE()

OpenResourceDialog::OpenResourceDialog(wxWindow* parent, IManager* manager, const wxString& initialSelection)
    : OpenResourceDialogBase(parent)
    , m_manager(manager)
    , m_needRefresh(false)
{
    Hide();
    BitmapLoader* bmpLoader = m_manager->GetStdIcons();

    m_tagImgMap[wxT("class")] = bmpLoader->LoadBitmap(wxT("cc/16/class"));
    m_tagImgMap[wxT("struct")] = bmpLoader->LoadBitmap(wxT("cc/16/struct"));
    m_tagImgMap[wxT("namespace")] = bmpLoader->LoadBitmap(wxT("cc/16/namespace"));
    m_tagImgMap[wxT("typedef")] = bmpLoader->LoadBitmap(wxT("cc/16/typedef"));
    m_tagImgMap[wxT("member_private")] = bmpLoader->LoadBitmap(wxT("cc/16/member_private"));
    m_tagImgMap[wxT("member_public")] = bmpLoader->LoadBitmap(wxT("cc/16/member_public"));
    m_tagImgMap[wxT("member_protected")] = bmpLoader->LoadBitmap(wxT("cc/16/member_protected"));
    m_tagImgMap[wxT("function_private")] = bmpLoader->LoadBitmap(wxT("cc/16/function_private"));
    m_tagImgMap[wxT("function_public")] = bmpLoader->LoadBitmap(wxT("cc/16/function_public"));
    m_tagImgMap[wxT("function_protected")] = bmpLoader->LoadBitmap(wxT("cc/16/function_protected"));
    m_tagImgMap[wxT("enum")] = bmpLoader->LoadBitmap(wxT("cc/16/enum"));
    m_tagImgMap[wxT("enumerator")] = bmpLoader->LoadBitmap(wxT("cc/16/enumerator"));
    m_tagImgMap[wxT("cpp")] = bmpLoader->LoadBitmap(wxT("mime/16/cpp"));
    m_tagImgMap[wxT("h")] = bmpLoader->LoadBitmap(wxT("mime/16/h"));
    m_tagImgMap[wxT("text")] = bmpLoader->LoadBitmap(wxT("mime/16/text"));
    m_tagImgMap[wxT("c")] = bmpLoader->LoadBitmap(wxT("mime/16/c"));
    m_tagImgMap[wxT("wxfb")] = bmpLoader->LoadBitmap(wxT("mime/16/wxfb"));
    m_tagImgMap[wxT("wxcp")] = bmpLoader->LoadBitmap(wxT("mime/16/wxcp"));

    m_timer = new wxTimer(this, XRCID("OR_TIMER"));

    m_textCtrlResourceName->SetFocus();
    SetLabel(_("Open resource..."));

    SetMinClientSize(wxSize(600, 400));
    GetSizer()->Fit(this);

    SetName("OpenResourceDialog");
    WindowAttrManager::Load(this);

    // load all files from the workspace
    if(m_manager->IsWorkspaceOpen()) {
        wxArrayString projects;
        m_manager->GetWorkspace()->GetProjectList(projects);

        for(size_t i = 0; i < projects.GetCount(); i++) {
            std::vector<wxFileName> fileNames;
            wxString errmsg;
            ProjectPtr p = m_manager->GetWorkspace()->FindProjectByName(projects.Item(i), errmsg);

            if(p) {
                p->GetFiles(fileNames, true);

                // convert std::vector to wxArrayString
                for(std::vector<wxFileName>::iterator it = fileNames.begin(); it != fileNames.end(); it++) {
                    wxString name = it->GetFullName().MakeLower();
                    m_files.insert(std::make_pair(name, it->GetFullPath()));
                }
            }
        }
    }

    // Set the initial selection
    // We use here 'SetValue' so an event will get fired and update the control
    if(!initialSelection.IsEmpty()) {
        m_textCtrlResourceName->SetValue(initialSelection);
        m_textCtrlResourceName->SelectAll();
    }

    m_dataview->GetColumn(0)->SetWidth(400);
    m_dataview->GetColumn(1)->SetWidth(60);
    m_dataview->GetColumn(2)->SetWidth(500);

    bool showFiles = clConfig::Get().Read("OpenResourceDialog/ShowFiles", true);
    bool showSymbols = clConfig::Get().Read("OpenResourceDialog/ShowSymbols", true);
    m_checkBoxFiles->SetValue(showFiles);
    m_checkBoxShowSymbols->SetValue(showSymbols);
    CentreOnParent();
}

OpenResourceDialog::~OpenResourceDialog()
{
    m_timer->Stop();
    wxDELETE(m_timer);

    clConfig::Get().Write("OpenResourceDialog/ShowFiles", m_checkBoxFiles->IsChecked());
    clConfig::Get().Write("OpenResourceDialog/ShowSymbols", m_checkBoxShowSymbols->IsChecked());
}

void OpenResourceDialog::OnText(wxCommandEvent& event)
{
    event.Skip();
    m_timer->Stop();
    m_timer->Start(200, true);

    wxString filter = m_textCtrlResourceName->GetValue();
    filter.Trim().Trim(false);

    if(filter.IsEmpty()) {
        // The filter content is cleared, delete all entries
        Clear();
        m_needRefresh = false;

    } else {
        m_needRefresh = true;
    }
}

void OpenResourceDialog::OnUsePartialMatching(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoPopulateList();
    m_textCtrlResourceName->SetFocus();
}

void OpenResourceDialog::OnEntryActivated(wxDataViewEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());

    m_dataviewModel->GetClientObject(event.GetItem());
    OpenResourceDialogItemData* data =
        dynamic_cast<OpenResourceDialogItemData*>(m_dataviewModel->GetClientObject(event.GetItem()));
    if(data) {
        EndModal(wxID_OK);
    }
}

void OpenResourceDialog::DoPopulateList()
{
    wxString name = m_textCtrlResourceName->GetValue();
    name.Trim().Trim(false);
    if(name.IsEmpty()) return;

    Clear();

    wxWindowUpdateLocker locker(m_dataview);

    // First add the workspace files

    // Prepare the user filter
    m_userFilters.Clear();
    m_userFilters = ::wxStringTokenize(name, " \t", wxTOKEN_STRTOK);
    for(size_t i = 0; i < m_userFilters.GetCount(); ++i) {
        m_userFilters.Item(i).MakeLower();
    }

    if(m_checkBoxFiles->IsChecked()) {
        DoPopulateWorkspaceFile();
    }

    if(m_checkBoxShowSymbols->IsChecked()) {
        DoPopulateTags();
    }
}

void OpenResourceDialog::DoPopulateTags()
{
    bool gotExactMatch(false);

    // Next, add the tags
    TagEntryPtrVector_t tags;
    if(m_userFilters.IsEmpty()) return;

    m_manager->GetTagsManager()->GetTagsByPartialName(m_userFilters.Item(0), tags);

    for(size_t i = 0; i < tags.size(); i++) {
        TagEntryPtr tag = tags.at(i);

        // Filter out non relevanting entries
        if(!m_filters.IsEmpty() && m_filters.Index(tag->GetKind()) == wxNOT_FOUND) continue;

        if(!MatchesFilter(tag->GetName())) continue;

        wxString name(tag->GetName());

        // keep the fullpath
        wxDataViewItem item;
        wxString fullname;
        if(tag->GetKind() == wxT("function") || tag->GetKind() == wxT("prototype")) {
            fullname = wxString::Format(
                wxT("%s::%s%s"), tag->GetScope().c_str(), tag->GetName().c_str(), tag->GetSignature().c_str());
            item = DoAppendLine(tag->GetName(), fullname, (tag->GetKind() == wxT("function")),
                new OpenResourceDialogItemData(
                                    tag->GetFile(), tag->GetLine(), tag->GetPattern(), tag->GetName(), tag->GetScope()),
                DoGetTagImg(tag));
        } else {

            fullname = wxString::Format(wxT("%s::%s"), tag->GetScope().c_str(), tag->GetName().c_str());
            item = DoAppendLine(tag->GetName(), fullname, false,
                new OpenResourceDialogItemData(
                                    tag->GetFile(), tag->GetLine(), tag->GetPattern(), tag->GetName(), tag->GetScope()),
                DoGetTagImg(tag));
        }

        if((m_userFilters.GetCount() == 1) && (m_userFilters.Item(0).CmpNoCase(name) == 0) && !gotExactMatch) {
            gotExactMatch = true;
            DoSelectItem(item);
        }
    }
}

void OpenResourceDialog::DoPopulateWorkspaceFile()
{
    // do we need to include files?
    if(!m_filters.IsEmpty() && m_filters.Index(TagEntry::KIND_FILE) == wxNOT_FOUND) return;

    if(!m_userFilters.IsEmpty()) {

        std::multimap<wxString, wxString>::iterator iter = m_files.begin();
        const int maxFileSize = 100;
        int counter = 0;
        for(; (iter != m_files.end()) && (counter < maxFileSize); iter++) {

            if(!MatchesFilter(iter->second)) continue;

            wxFileName fn(iter->second);
            FileExtManager::FileType type = FileExtManager::GetType(fn.GetFullName());
            wxBitmap imgId = m_tagImgMap[wxT("text")];
            switch(type) {
            case FileExtManager::TypeSourceC:
                imgId = m_tagImgMap[wxT("c")];
                break;

            case FileExtManager::TypeSourceCpp:
                imgId = m_tagImgMap[wxT("cpp")];
                break;
            case FileExtManager::TypeHeader:
                imgId = m_tagImgMap[wxT("h")];
                break;
            case FileExtManager::TypeFormbuilder:
                imgId = m_tagImgMap[wxT("wxfb")];
                break;
            case FileExtManager::TypeWxCrafter:
                imgId = m_tagImgMap[wxT("wxcp")];
                break;
            default:
                break;
            }
            DoAppendLine(fn.GetFullName(), fn.GetFullPath(), false,
                new OpenResourceDialogItemData(fn.GetFullPath(), -1, wxT(""), fn.GetFullName(), wxT("")), imgId);
            ++counter;
        }
    }
}

void OpenResourceDialog::Clear()
{
    // list control does not own the client data, we need to free it ourselves
    m_dataviewModel->Clear();
    m_userFilters.Clear();
}

void OpenResourceDialog::OpenSelection(const OpenResourceDialogItemData& selection, IManager* manager)
{
    // send event to the plugins to see if they want
    // to open this file
    wxString file_path = selection.m_file;
    clCommandEvent activateEvent(wxEVT_TREE_ITEM_FILE_ACTIVATED);
    activateEvent.SetFileName(file_path);
    if(EventNotifier::Get()->ProcessEvent(activateEvent)) return;

    if(manager && manager->OpenFile(selection.m_file, wxEmptyString, selection.m_line - 1)) {
        IEditor* editor = manager->GetActiveEditor();
        if(editor && !selection.m_name.IsEmpty() && !selection.m_pattern.IsEmpty()) {
            editor->FindAndSelectV(selection.m_pattern, selection.m_name);
        }
    }
}

void OpenResourceDialog::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    if(m_dataviewModel->IsEmpty()) return;

    if(event.GetKeyCode() == WXK_DOWN || event.GetKeyCode() == WXK_UP || event.GetKeyCode() == WXK_NUMPAD_UP ||
        event.GetKeyCode() == WXK_NUMPAD_DOWN) {
        event.Skip(false);
        bool down = (event.GetKeyCode() == WXK_DOWN || event.GetKeyCode() == WXK_NUMPAD_DOWN);
        wxDataViewItemArray children;
        m_dataviewModel->GetChildren(wxDataViewItem(0), children);
        wxDataViewItem selection = m_dataview->GetSelection();
        if(!selection.IsOk()) {
            // No selection, select the first
            DoSelectItem(children.Item(0));
        } else {
            int curIndex = wxNOT_FOUND;
            for(size_t i = 0; i < children.size(); ++i) {
                if(children.Item(i) == selection) {
                    curIndex = i;
                    break;
                }
            }

            if(curIndex != wxNOT_FOUND) {
                down ? ++curIndex : --curIndex;
                if((curIndex >= 0) && (curIndex < (int)children.size())) {
                    DoSelectItem(children.Item(curIndex));
                }
            }
        }

        // Set the focus back to the text control
        m_textCtrlResourceName->CallAfter(&wxTextCtrl::SetFocus);
    }
}

void OpenResourceDialog::OnOK(wxCommandEvent& event) { event.Skip(); }

void OpenResourceDialog::OnOKUI(wxUpdateUIEvent& event)
{
    wxDataViewItem item = m_dataview->GetSelection();
    event.Enable(item.IsOk());
}

bool OpenResourceDialogItemData::IsOk() const { return m_file.IsEmpty() == false; }

void OpenResourceDialog::DoSelectItem(const wxDataViewItem& item)
{
    CHECK_ITEM_RET(item);
    m_dataview->Select(item);
    m_dataview->EnsureVisible(item);
}

wxDataViewItem OpenResourceDialog::DoAppendLine(const wxString& name,
    const wxString& fullname,
    bool boldFont,
    OpenResourceDialogItemData* clientData,
    const wxBitmap& bmp)
{
    wxString prefix;
    clientData->m_impl = boldFont;
    wxVector<wxVariant> cols;
    cols.push_back(OpenResourceDialogModel::CreateIconTextVariant(prefix + name, bmp));
    cols.push_back(clientData->m_impl ? wxString("X") : wxString());
    cols.push_back(fullname);
    return m_dataviewModel->AppendItem(wxDataViewItem(0), cols, clientData);
}

void OpenResourceDialog::OnTimer(wxTimerEvent& event)
{
    if(m_needRefresh) DoPopulateList();

    m_needRefresh = false;
}

wxBitmap OpenResourceDialog::DoGetTagImg(TagEntryPtr tag)
{
    wxString kind = tag->GetKind();
    wxString access = tag->GetAccess();
    wxBitmap bmp = m_tagImgMap[wxT("text")];
    if(kind == wxT("class")) bmp = m_tagImgMap[wxT("class")];

    if(kind == wxT("struct")) bmp = m_tagImgMap[wxT("struct")];

    if(kind == wxT("namespace")) bmp = m_tagImgMap[wxT("namespace")];

    if(kind == wxT("variable")) bmp = m_tagImgMap[wxT("member_public")];

    if(kind == wxT("typedef")) bmp = m_tagImgMap[wxT("typedef")];

    if(kind == wxT("member") && access.Contains(wxT("private"))) bmp = m_tagImgMap[wxT("member_private")];

    if(kind == wxT("member") && access.Contains(wxT("public"))) bmp = m_tagImgMap[wxT("member_public")];

    if(kind == wxT("member") && access.Contains(wxT("protected"))) bmp = m_tagImgMap[wxT("member_protected")];

    if(kind == wxT("member")) bmp = m_tagImgMap[wxT("member_public")];

    if((kind == wxT("function") || kind == wxT("prototype")) && access.Contains(wxT("private")))
        bmp = m_tagImgMap[wxT("function_private")];

    if((kind == wxT("function") || kind == wxT("prototype")) && (access.Contains(wxT("public")) || access.IsEmpty()))
        bmp = m_tagImgMap[wxT("function_public")];

    if((kind == wxT("function") || kind == wxT("prototype")) && access.Contains(wxT("protected")))
        bmp = m_tagImgMap[wxT("function_protected")];

    if(kind == wxT("macro")) bmp = m_tagImgMap[wxT("typedef")];

    if(kind == wxT("enum")) bmp = m_tagImgMap[wxT("enum")];

    if(kind == wxT("enumerator")) bmp = m_tagImgMap[wxT("enumerator")];

    return bmp;
}

bool OpenResourceDialog::MatchesFilter(const wxString& name)
{
    wxString tmpname = name;
    tmpname.MakeLower();

    if(m_userFilters.IsEmpty()) return false;

    for(size_t i = 0; i < m_userFilters.GetCount(); ++i) {
        if(!tmpname.Contains(m_userFilters.Item(i))) return false;
    }
    return true;
}
void OpenResourceDialog::OnCheckboxfilesCheckboxClicked(wxCommandEvent& event) { DoPopulateList(); }
void OpenResourceDialog::OnCheckboxshowsymbolsCheckboxClicked(wxCommandEvent& event) { DoPopulateList(); }

void OpenResourceDialog::OnEnter(wxCommandEvent& event)
{
    wxDataViewItem item = m_dataview->GetSelection();

    if(item.IsOk()) {
        EndModal(wxID_OK);
    }
}

void OpenResourceDialog::OnEntrySelected(wxDataViewEvent& event) { event.Skip(); }

OpenResourceDialogItemData* OpenResourceDialog::GetSelection() const
{
    wxDataViewItem item = m_dataview->GetSelection();
    if(!item.IsOk()) return NULL;

    OpenResourceDialogItemData* data =
        dynamic_cast<OpenResourceDialogItemData*>(m_dataviewModel->GetClientObject(item));
    return data;
}
