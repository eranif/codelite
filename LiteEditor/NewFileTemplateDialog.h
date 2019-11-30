#ifndef NEWFILETEMPLATEDIALOG_H
#define NEWFILETEMPLATEDIALOG_H
#include "compiler_pages.h"

class NewFileTemplateDialog : public NewFileTemplateDialogBase
{
public:
    NewFileTemplateDialog(wxWindow* parent);
    virtual ~NewFileTemplateDialog();
    
    wxString GetPattern() const;
    wxString GetKind() const;
    wxString GetExtension() const;
    
    void SetPattern(const wxString& s);
    void SetKind(const wxString& k);
    void SetExtension(const wxString& ext);
};
#endif // NEWFILETEMPLATEDIALOG_H
