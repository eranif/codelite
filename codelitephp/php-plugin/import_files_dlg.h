#ifndef __import_files_dlg__
#define __import_files_dlg__

#include "php_ui.h"

class ImportFilesDlg : public ImportFilesBaseDlg
{
public:
    /** Constructor */
    ImportFilesDlg( wxWindow* parent );
    virtual ~ImportFilesDlg();

    bool GetIsRecursive() const {
        return m_checkBoxSubDirs->IsChecked();
    }

    wxString GetPath() const {
        return m_textCtrlFolderPath->GetValue();
    }

    wxString GetFileSpec() const {
        return m_textCtrlFileExtension->GetValue();
    }
protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
    virtual void OnBrowse(wxCommandEvent& event);
};

#endif // __import_files_dlg__
