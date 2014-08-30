#ifndef FILEMAPPINGDLG_H
#define FILEMAPPINGDLG_H
#include "php_ui.h"

class FileMappingDlg : public FileMappingDlgBase
{
public:
    FileMappingDlg(wxWindow* parent);
    virtual ~FileMappingDlg();
    
    wxString GetSourceFolder() const {
        return m_dirPickerSource->GetPath();
    }
    
    wxString GetTargetFolder() const {
        return m_dirPickerTarget->GetPath();
    }
    
    void SetSourceFolder( const wxString &path) {
        m_dirPickerSource->SetPath( path );
    }
    
    void SetTargetFolder( const wxString &path) {
        m_dirPickerTarget->SetPath( path );
    }
    
protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
};
#endif // FILEMAPPINGDLG_H
