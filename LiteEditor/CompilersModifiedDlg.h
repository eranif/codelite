#ifndef COMPILERSMODIFIEDDLG_H
#define COMPILERSMODIFIEDDLG_H

#include "CompilersFoundDlgBase.h"
#include <macros.h>
#include <wx/vector.h>
#include <wx/propgrid/property.h>

class CompilersModifiedDlg : public CompilersModifiedDlgBase
{
    wxVector<wxPGProperty*> m_props;
    wxStringMap_t           m_table;
    bool                    m_enableOKButton;
    
public:
    CompilersModifiedDlg(wxWindow* parent, const wxStringSet_t& deletedCompilers);
    virtual ~CompilersModifiedDlg();
    
    const wxStringMap_t& GetReplacementTable() const {
        return m_table;
    }
    
protected:
    virtual void OnValueChanged(wxPropertyGridEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // COMPILERSMODIFIEDDLG_H
