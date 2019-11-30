#ifndef EDITCMPTEMPLATEDIALOG_H
#define EDITCMPTEMPLATEDIALOG_H
#include "compiler_pages.h"

class EditCmpTemplateDialog : public EditCmpTemplateDialogBase
{
public:
    EditCmpTemplateDialog(wxWindow* parent);
    virtual ~EditCmpTemplateDialog();
    
    void SetPattern(const wxString& pattern);
    wxString GetPattern() const;
};
#endif // EDITCMPTEMPLATEDIALOG_H
