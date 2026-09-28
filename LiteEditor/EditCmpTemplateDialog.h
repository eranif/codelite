#ifndef EDITCMPTEMPLATEDIALOG_H
#define EDITCMPTEMPLATEDIALOG_H
#include "compiler_pages.hpp"

class EditCmpTemplateDialog : public EditCmpTemplateDialogBase
{
public:
    EditCmpTemplateDialog(wxWindow* parent);
    ~EditCmpTemplateDialog() override = default;

    void SetPattern(const wxString& pattern);
    wxString GetPattern() const;
};
#endif // EDITCMPTEMPLATEDIALOG_H
