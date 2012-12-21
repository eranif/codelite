#ifndef IMPORTFILESDIALOGNEW_H
#define IMPORTFILESDIALOGNEW_H
#include "importfilesdialog_new.h"
#include <wx/dir.h>

class ImportFilesDialogNew : public ImportFilesDialogNewBase
{
    wxDataViewItem m_root;
    wxVariant MakeIconText(const wxString &text, const wxBitmap& bmp) const;

protected:
    virtual void OnItemExpanding(wxDataViewEvent& event);
    void DoCheckChildren(const wxDataViewItem& parent, bool check);
    void DoGetCheckedDirs(const wxDataViewItem& parent, wxArrayString& dirs);

protected:
    virtual void OnValueChanged(wxDataViewEvent& event);
    virtual void OnDirChanged(wxFileDirPickerEvent& event);
    void DoBuildTree(const wxDataViewItem& parent, const wxDir& dir, bool initialState);
    void DoBuildTree();

public:
    ImportFilesDialogNew(wxWindow* parent);
    virtual ~ImportFilesDialogNew();
    void GetDirectories(wxArrayString &dirs);
    bool ExtlessFiles();
    wxString GetFileMask();
    wxString GetBaseDir();

};
#endif // IMPORTFILESDIALOGNEW_H
