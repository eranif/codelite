#ifndef FOLDERMAPPINGDLG_H
#define FOLDERMAPPINGDLG_H
#include "UI.h"
#include "LLDBProtocol/LLDBPivot.h"

class FolderMappingDlg : public FolderMappingBaseDlg
{
public:
    FolderMappingDlg(wxWindow* parent);
    virtual ~FolderMappingDlg();
    LLDBPivot GetPivot() const;
    
protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
    
};
#endif // FOLDERMAPPINGDLG_H
