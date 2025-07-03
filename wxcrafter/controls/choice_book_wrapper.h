#ifndef CHOICEBOOKWRAPPER_H
#define CHOICEBOOKWRAPPER_H

#include "notebook_base_wrapper.h"

class ChoiceBookWrapper : public NotebookBaseWrapper
{
public:
    ChoiceBookWrapper();
    ~ChoiceBookWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    wxString GetXRCPageClass() const override;
};

#endif // CHOICEBOOKWRAPPER_H
