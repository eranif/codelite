//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ImportFilesDialogNew.cpp
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

#include "ImportFilesDialogNew.h"
#include <wx/dir.h>
#include <wx/filefn.h>
#include "pluginmanager.h"
#include "bitmap_loader.h"
#include "importfilessettings.h"
#include "ctags_manager.h"
#include "pluginmanager.h"
#include "windowattrmanager.h"
#include "editor_config.h"
#include <wx/dirdlg.h>

class ImportFilesDlgData : public wxClientData
{
    wxString m_path;
    bool m_isChecked;
    bool m_isDummy;

public:
    ImportFilesDlgData(const wxString& path, bool checked, bool isDummy = false)
        : m_path(path)
        , m_isChecked(checked)
        , m_isDummy(isDummy)
    {
    }

    virtual ~ImportFilesDlgData() {}

    void SetIsChecked(bool isChecked) { this->m_isChecked = isChecked; }
    void SetPath(const wxString& path) { this->m_path = path; }
    bool IsChecked() const { return m_isChecked; }
    const wxString& GetPath() const { return m_path; }
    void SetIsDummy(bool isDummy) { this->m_isDummy = isDummy; }
    bool IsDummy() const { return m_isDummy; }
};

//--------------------------------------------------------------

ImportFilesDialogNew::ImportFilesDialogNew(wxWindow* parent)
    : ImportFilesDialogNewBase(parent)
{
    m_dataview->SetExpanderColumn(m_dataview->GetColumn(1));
    m_dataview->SetIndent(WXC_FROM_DIP(16));
    ImportFilesSettings options;
    if(!EditorConfigST::Get()->ReadObject(wxT("import_dir_options"), &options)) {
        // first time, read the settings from the ctags options
        options.SetFileMask(TagsManagerST::Get()->GetCtagsOptions().GetFileSpec());
        bool noExt = TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_PARSE_EXT_LESS_FILES ? true : false;
        size_t flags(0);
        if(noExt) {
            flags |= IFS_INCLUDE_FILES_WO_EXT;
        }
        options.SetFlags(flags);
    }

    options.SetBaseDir(PluginManager::Get()->GetSelectedTreeItemInfo(TreeFileView).m_fileName.GetPath());
    if(options.GetBaseDir().IsEmpty()) {
        options.SetBaseDir(wxGetCwd());
    }

    m_textCtrlDir->ChangeValue(options.GetBaseDir());
    m_textCtrSpec->SetValue(options.GetFileMask());
    m_checkBoxFilesWOExt->SetValue(options.GetFlags() & IFS_INCLUDE_FILES_WO_EXT);
    SetName("ImportFilesDialogNew");
    WindowAttrManager::Load(this);

    CentreOnParent();
    DoBuildTree();
}

ImportFilesDialogNew::~ImportFilesDialogNew()
{

    ImportFilesSettings options;
    options.SetBaseDir(m_textCtrlDir->GetValue());
    options.SetFileMask(m_textCtrSpec->GetValue());

    size_t flags(0);
    if(m_checkBoxFilesWOExt->IsChecked()) flags |= IFS_INCLUDE_FILES_WO_EXT;
    options.SetFlags(flags);
    EditorConfigST::Get()->WriteObject(wxT("import_dir_options"), &options);
}

void ImportFilesDialogNew::DoBuildTree(const wxDataViewItem& parent, const wxDir& dir, bool initialState)
{
    wxString path;
    bool cont = dir.GetFirst(&path, "", wxDIR_DIRS);
    wxBitmap folderBmp = PluginManager::Get()->GetStdIcons()->LoadBitmap("mime/16/folder-yellow");
    while(cont) {

        wxVector<wxVariant> cols;
        cols.push_back(initialState);
        cols.push_back(MakeIconText(path, folderBmp));

        wxDir childDir(dir.GetNameWithSep() + path);
        wxDataViewItem child =
            m_dataviewModel->AppendItem(parent, cols, new ImportFilesDlgData(childDir.GetName(), initialState));

        // Add dummy columns
        if(childDir.IsOpened() && childDir.HasSubDirs()) {
            wxVector<wxVariant> dummyCols;
            dummyCols.push_back(false);
            dummyCols.push_back(MakeIconText("dummy", folderBmp));
            m_dataviewModel->AppendItem(child, dummyCols, new ImportFilesDlgData("", false, true));
        }
        cont = dir.GetNext(&path);
    }
}

void ImportFilesDialogNew::OnDirChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoBuildTree();
}

void ImportFilesDialogNew::DoBuildTree()
{
    if(!wxFileName::DirExists(m_textCtrlDir->GetValue())) return;
    m_dataviewModel->Clear();

    wxString curpath = m_textCtrlDir->GetValue();
    if(!wxDir::Exists(curpath)) {
        m_textCtrlDir->ChangeValue(::wxGetCwd());
    }

    wxVector<wxVariant> cols;
    cols.push_back(false);
    cols.push_back(MakeIconText(m_textCtrlDir->GetValue(),
                                PluginManager::Get()->GetStdIcons()->LoadBitmap("mime/16/folder-yellow")));

    m_root =
        m_dataviewModel->AppendItem(wxDataViewItem(0), cols, new ImportFilesDlgData(m_textCtrlDir->GetValue(), false));

    // For performance, we add only the direct children of the root node
    wxDir dir(m_textCtrlDir->GetValue());
    if(dir.IsOpened() && dir.HasSubDirs()) {
        DoBuildTree(m_root, dir, false);
    }
    m_dataview->Expand(m_root);
}

void ImportFilesDialogNew::OnValueChanged(wxDataViewEvent& event)
{
    event.Skip();
    wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData(event.GetItem());
    if(cols.size() > 1) {
        bool isChecked = cols.at(0).GetBool();
        ImportFilesDlgData* cd =
            reinterpret_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(event.GetItem()));
        if(cd) {
            cd->SetIsChecked(isChecked);
            DoCheckChildren(event.GetItem(), isChecked);
        }
#ifdef __WXGTK__
        Refresh();
#endif
    }
}

wxVariant ImportFilesDialogNew::MakeIconText(const wxString& text, const wxBitmap& bmp) const
{
    wxIcon icn;
    icn.CopyFromBitmap(bmp);
    wxDataViewIconText ict(text, icn);
    wxVariant v;
    v << ict;
    return v;
}

void ImportFilesDialogNew::DoCheckChildren(const wxDataViewItem& parent, bool check)
{
    if(m_dataviewModel->HasChildren(parent)) {
        wxDataViewItemArray children;
        m_dataviewModel->GetChildren(parent, children);
        for(size_t i = 0; i < children.GetCount(); ++i) {

            // First, update the UI by replacing the columns
            wxDataViewItem item = children.Item(i);
            m_dataviewModel->SetValue(wxVariant(check), item, 0);

            // Update the client data
            ImportFilesDlgData* cd = dynamic_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(item));
            if(cd) {
                cd->SetIsChecked(check);
            }

            // Check if this child has children
            if(m_dataviewModel->HasChildren(item)) {
                DoCheckChildren(item, check);
            }
        }
    }
}

void ImportFilesDialogNew::OnItemExpanding(wxDataViewEvent& event)
{
    event.Skip();
    wxDataViewItemArray children;
    if(m_dataviewModel->GetChildren(event.GetItem(), children)) {
        wxDataViewItem child = children.Item(0);
        ImportFilesDlgData* cd = dynamic_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(child));
        if(cd && cd->IsDummy()) {
            cd = dynamic_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(event.GetItem()));
            m_dataviewModel->DeleteItem(child);
            wxDir dir(cd->GetPath());
            DoBuildTree(event.GetItem(), dir, cd->IsChecked());
        }
    }
}

void ImportFilesDialogNew::GetDirectories(wxStringBoolMap_t& dirs) { DoGetCheckedDirs(m_root, dirs); }

void ImportFilesDialogNew::DoGetCheckedDirs(const wxDataViewItem& parent, wxStringBoolMap_t& dirs)
{
    wxDataViewItemArray children;
    bool itemExpanded = false;
    ImportFilesDlgData* cd = dynamic_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(parent));
    if(cd) {
        bool bParentFolderChecked = cd->IsChecked();
        wxString dirname = cd->GetPath();
        bool recurse = false;
        // check if this item was expanded before
        if(m_dataviewModel->HasChildren(parent) && m_dataviewModel->GetChildren(parent, children)) {
            wxDataViewItem child = children.Item(0);
            cd = dynamic_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(child));

            // If the directory is checked and it was never expanded
            // we should recurse into it
            if(cd && cd->IsDummy()) {
                recurse = true;

            } else if(cd) {
                itemExpanded = true;
            }
        }

        if(bParentFolderChecked) {
            dirs.insert(std::make_pair(dirname, recurse));
        }
    }

    // if the parent has children and it was expanded by the user, keep on recursing
    if(!children.IsEmpty() && itemExpanded) {
        for(size_t i = 0; i < children.GetCount(); ++i) {
            DoGetCheckedDirs(children.Item(i), dirs);
        }
    }
}

bool ImportFilesDialogNew::ExtlessFiles() { return m_checkBoxFilesWOExt->IsChecked(); }

wxString ImportFilesDialogNew::GetBaseDir() { return m_textCtrlDir->GetValue(); }

wxString ImportFilesDialogNew::GetFileMask() { return m_textCtrSpec->GetValue(); }
void ImportFilesDialogNew::OnBrowse(wxCommandEvent& event)
{
    wxString new_path = wxDirSelector(_("Select working directory:"), m_textCtrlDir->GetValue(), wxDD_DEFAULT_STYLE,
                                      wxDefaultPosition, this);
    if(new_path.IsEmpty()) return;
    m_textCtrlDir->ChangeValue(new_path);
    DoBuildTree();
}
