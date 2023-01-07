#ifndef CHOICEBOOKWRAPPER_H
#define CHOICEBOOKWRAPPER_H

#include "notebook_base_wrapper.h"

class ChoiceBookWrapper : public NotebookBaseWrapper
{
public:
    ChoiceBookWrapper();
    virtual ~ChoiceBookWrapper();

public:
    virtual wxcWidget* Clone() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    wxString GetXRCPageClass() const;
};

#endif // CHOICEBOOKWRAPPER_H
