#include "BitmapSelectorDlg.h"
#include "wxc_project_metadata.h"
#include "wxc_settings.h"
#include "wxgui_helpers.h"
#include <windowattrmanager.h>
#include <wx/filedlg.h>
#include <wx/filename.h>

BitmapSelectorDlg::BitmapSelectorDlg(wxWindow* parent, const wxString& selection)
    : BitmapSelectorDlgBase(parent)
{
    wxArrayString arr;
    arr.Add(wxT("wxART_ERROR"));
    arr.Add(wxT("wxART_QUESTION"));
    arr.Add(wxT("wxART_WARNING"));
    arr.Add(wxT("wxART_INFORMATION"));
    arr.Add(wxT("wxART_ADD_BOOKMARK"));
    arr.Add(wxT("wxART_DEL_BOOKMARK"));
    arr.Add(wxT("wxART_HELP_SIDE_PANEL"));
    arr.Add(wxT("wxART_HELP_SETTINGS"));
    arr.Add(wxT("wxART_HELP_BOOK"));
    arr.Add(wxT("wxART_HELP_FOLDER"));
    arr.Add(wxT("wxART_HELP_PAGE"));
    arr.Add(wxT("wxART_GO_BACK"));
    arr.Add(wxT("wxART_GO_FORWARD"));
    arr.Add(wxT("wxART_GO_UP"));
    arr.Add(wxT("wxART_GO_DOWN"));
    arr.Add(wxT("wxART_GO_TO_PARENT"));
    arr.Add(wxT("wxART_GO_HOME"));
    arr.Add(wxT("wxART_GOTO_FIRST"));
    arr.Add(wxT("wxART_GOTO_LAST"));
    arr.Add(wxT("wxART_PRINT"));
    arr.Add(wxT("wxART_HELP"));
    arr.Add(wxT("wxART_TIP"));
    arr.Add(wxT("wxART_REPORT_VIEW"));
    arr.Add(wxT("wxART_LIST_VIEW"));
    arr.Add(wxT("wxART_NEW_DIR"));
    arr.Add(wxT("wxART_FOLDER"));
    arr.Add(wxT("wxART_FOLDER_OPEN"));
    arr.Add(wxT("wxART_GO_DIR_UP"));
    arr.Add(wxT("wxART_EXECUTABLE_FILE"));
    arr.Add(wxT("wxART_NORMAL_FILE"));
    arr.Add(wxT("wxART_TICK_MARK"));
    arr.Add(wxT("wxART_CROSS_MARK"));
    arr.Add(wxT("wxART_MISSING_IMAGE"));
    arr.Add(wxT("wxART_NEW"));
    arr.Add(wxT("wxART_FILE_OPEN"));
    arr.Add(wxT("wxART_FILE_SAVE"));
    arr.Add(wxT("wxART_FILE_SAVE_AS"));
    arr.Add(wxT("wxART_DELETE"));
    arr.Add(wxT("wxART_COPY"));
    arr.Add(wxT("wxART_CUT"));
    arr.Add(wxT("wxART_PASTE"));
    arr.Add(wxT("wxART_UNDO"));
    arr.Add(wxT("wxART_REDO"));
    arr.Add(wxT("wxART_PLUS"));
    arr.Add(wxT("wxART_MINUS"));
    arr.Add(wxT("wxART_CLOSE"));
    arr.Add(wxT("wxART_QUIT"));
    arr.Add(wxT("wxART_FIND"));
    arr.Add(wxT("wxART_FIND_AND_REPLACE"));
    arr.Add(wxT("wxART_HARDDISK"));
    arr.Add(wxT("wxART_FLOPPY"));
    arr.Add(wxT("wxART_CDROM"));
    arr.Add(wxT("wxART_REMOVABLE"));
    arr.Sort();

    m_choiceArtID->Append(arr);
    m_choiceArtID->SetSelection(0);
    m_checkBoxConvertToRelativePath->SetValue(!wxcSettings::Get().HasFlag(wxcSettings::DONT_USE_RELATIVE_BITMAPS));

    wxString artID, clientID, sizeHint;
    if(wxCrafter::IsArtProviderBitmap(selection, artID, clientID, sizeHint)) {

        int where = m_choiceArtClientID->FindString(clientID);
        if(where != wxNOT_FOUND) { m_choiceArtClientID->SetSelection(where); }

        where = m_choiceArtID->FindString(artID);
        if(where != wxNOT_FOUND) { m_choiceArtID->SetSelection(where); }

        where = m_choiceSize->FindString(sizeHint);
        if(where != wxNOT_FOUND) { m_choiceSize->SetSelection(where); }

        m_checkBoxSelectFile->SetValue(false);
        m_checkBoxArtProvider->SetValue(true);

    } else {
        // File path
        m_checkBoxSelectFile->SetValue(true);
        m_checkBoxArtProvider->SetValue(false);

        wxFileName fn(selection);
        if(!fn.IsRelative() && m_checkBoxConvertToRelativePath->IsChecked()) {
            fn.MakeRelativeTo(wxcProjectMetadata::Get().GetProjectPath());
        }
        m_textCtrlFile->ChangeValue(wxCrafter::ToUnixPath(fn.GetFullPath()));
    }
    SetName("BitmapSelectorDlg");
    WindowAttrManager::Load(this);
}

BitmapSelectorDlg::~BitmapSelectorDlg()
{
    wxcSettings::Get().EnableFlag(wxcSettings::DONT_USE_RELATIVE_BITMAPS,
                                  !m_checkBoxConvertToRelativePath->IsChecked());
}

void BitmapSelectorDlg::OnBrowseFile(wxCommandEvent& event)
{
    wxString initialFileValue = m_textCtrlFile->GetValue();
    wxFileName fnBitmap(initialFileValue);
    if(fnBitmap.IsRelative()) { fnBitmap.MakeAbsolute(wxcProjectMetadata::Get().GetProjectPath()); }
    initialFileValue = fnBitmap.GetFullPath();

    wxString wildcard = wxT("PNG files (*.png)|*.png|BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif|All files (*)|*");
    wxString newPath = ::wxFileSelector(_("Select bitmap"), fnBitmap.GetPath(), fnBitmap.GetFullName(), wxEmptyString,
                                        wildcard, wxFD_OPEN);

    if(newPath.IsEmpty() == false) {
        wxFileName newFilePath(newPath);
        if(m_checkBoxConvertToRelativePath->IsChecked()) {
            newFilePath.MakeRelativeTo(wxcProjectMetadata::Get().GetProjectPath());
        }
        m_textCtrlFile->ChangeValue(wxCrafter::ToUnixPath(newFilePath.GetFullPath()));
    }
}

void BitmapSelectorDlg::OnSelectBitmapUI(wxUpdateUIEvent& event) { event.Enable(m_checkBoxSelectFile->IsChecked()); }

void BitmapSelectorDlg::OnUserArtPorviderUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxArtProvider->IsChecked());
}

wxString BitmapSelectorDlg::GetBitmapFile() const
{
    if(m_checkBoxArtProvider->IsChecked()) {
        wxString str;
        wxString size = m_choiceSize->GetStringSelection();
        str << m_choiceArtID->GetStringSelection() << "," << m_choiceArtClientID->GetStringSelection() << "," << size;
        return str;

    } else {
        return m_textCtrlFile->GetValue();
    }
}

void BitmapSelectorDlg::OnUseFilePicker(wxCommandEvent& event) { m_checkBoxArtProvider->SetValue(false); }

void BitmapSelectorDlg::OnUserArtProvider(wxCommandEvent& event) { m_checkBoxSelectFile->SetValue(false); }
void BitmapSelectorDlg::OnUseRelativePaths(wxCommandEvent& event)
{
    wxFileName newFilePath(m_textCtrlFile->GetValue());
    if(event.IsChecked()) {
        newFilePath.MakeRelativeTo(wxcProjectMetadata::Get().GetProjectPath());
    } else {
        newFilePath.MakeAbsolute(wxcProjectMetadata::Get().GetProjectPath());
    }
    m_textCtrlFile->ChangeValue(wxCrafter::ToUnixPath(newFilePath.GetFullPath()));
}
