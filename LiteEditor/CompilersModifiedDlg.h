#ifndef COMPILERSMODIFIEDDLG_H
#define COMPILERSMODIFIEDDLG_H
#include "CompilersFoundDlgBase.h"
#include <macros.h>

class CompilersModifiedDlg : public CompilersModifiedDlgBase
{
public:
    CompilersModifiedDlg(wxWindow* parent, const wxStringSet_t& deletedCompilers);
    virtual ~CompilersModifiedDlg();
    
    wxStringMap_t GetReplacementTable() const;
protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // COMPILERSMODIFIEDDLG_H
