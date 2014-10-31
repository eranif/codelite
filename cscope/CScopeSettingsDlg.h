#ifndef CSCOPESETTINGSDLG_H
#define CSCOPESETTINGSDLG_H
#include "CscopeTabBase.h"

class CScopeSettingsDlg : public CScopeSettingsDlgBase
{
public:
    CScopeSettingsDlg(wxWindow* parent);
    virtual ~CScopeSettingsDlg();
    
    wxString GetPath() const { return m_filePickerCScopeExe->GetPath(); }
};
#endif // CSCOPESETTINGSDLG_H
