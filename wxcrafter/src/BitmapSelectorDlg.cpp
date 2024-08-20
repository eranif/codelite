#include "BitmapSelectorDlg.h"

#include "StdToWX.h"
#include "windowattrmanager.h"
#include "wxc_project_metadata.h"
#include "wxc_settings.h"
#include "wxgui_helpers.h"

#include <wx/filedlg.h>
#include <wx/filename.h>

BitmapSelectorDlg::BitmapSelectorDlg(wxWindow* parent, const wxString& selection)
    : BitmapSelectorDlgBase(parent)
{
    wxArrayString arr = StdToWX::ToArrayString({ wxT("wxART_ERROR"),
                                                 wxT("wxART_QUESTION"),
                                                 wxT("wxART_WARNING"),
                                                 wxT("wxART_INFORMATION"),
                                                 wxT("wxART_ADD_BOOKMARK"),
                                                 wxT("wxART_DEL_BOOKMARK"),
                                                 wxT("wxART_HELP_SIDE_PANEL"),
                                                 wxT("wxART_HELP_SETTINGS"),
                                                 wxT("wxART_HELP_BOOK"),
                                                 wxT("wxART_HELP_FOLDER"),
                                                 wxT("wxART_HELP_PAGE"),
                                                 wxT("wxART_GO_BACK"),
                                                 wxT("wxART_GO_FORWARD"),
                                                 wxT("wxART_GO_UP"),
                                                 wxT("wxART_GO_DOWN"),
                                                 wxT("wxART_GO_TO_PARENT"),
                                                 wxT("wxART_GO_HOME"),
                                                 wxT("wxART_GOTO_FIRST"),
                                                 wxT("wxART_GOTO_LAST"),
                                                 wxT("wxART_PRINT"),
                                                 wxT("wxART_HELP"),
                                                 wxT("wxART_TIP"),
                                                 wxT("wxART_REPORT_VIEW"),
                                                 wxT("wxART_LIST_VIEW"),
                                                 wxT("wxART_NEW_DIR"),
                                                 wxT("wxART_FOLDER"),
                                                 wxT("wxART_FOLDER_OPEN"),
                                                 wxT("wxART_GO_DIR_UP"),
                                                 wxT("wxART_EXECUTABLE_FILE"),
                                                 wxT("wxART_NORMAL_FILE"),
                                                 wxT("wxART_TICK_MARK"),
                                                 wxT("wxART_CROSS_MARK"),
                                                 wxT("wxART_MISSING_IMAGE"),
                                                 wxT("wxART_NEW"),
                                                 wxT("wxART_FILE_OPEN"),
                                                 wxT("wxART_FILE_SAVE"),
                                                 wxT("wxART_FILE_SAVE_AS"),
                                                 wxT("wxART_DELETE"),
                                                 wxT("wxART_COPY"),
                                                 wxT("wxART_CUT"),
                                                 wxT("wxART_PASTE"),
                                                 wxT("wxART_UNDO"),
                                                 wxT("wxART_REDO"),
                                                 wxT("wxART_PLUS"),
                                                 wxT("wxART_MINUS"),
                                                 wxT("wxART_CLOSE"),
                                                 wxT("wxART_QUIT"),
                                                 wxT("wxART_FIND"),
                                                 wxT("wxART_FIND_AND_REPLACE"),
                                                 wxT("wxART_HARDDISK"),
                                                 wxT("wxART_FLOPPY"),
                                                 wxT("wxART_CDROM"),
                                                 wxT("wxART_REMOVABLE") });
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
