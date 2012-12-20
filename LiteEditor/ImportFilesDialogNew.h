#ifndef IMPORTFILESDIALOGNEW_H
#define IMPORTFILESDIALOGNEW_H
#include "importfilesdialog_new.h"
#include <wx/dir.h>

class ImportFilesDialogNew : public ImportFilesDialogNewBase
{
    wxDataViewItem m_root;
protected:
    virtual void OnDirChanged(wxFileDirPickerEvent& event);
    void DoBuildTree(const wxDataViewItem& parent, const wxDir& dir);
    void DoBuildTree();
    
public:
    ImportFilesDialogNew(wxWindow* parent);
    virtual ~ImportFilesDialogNew();
};
#endif // IMPORTFILESDIALOGNEW_H
