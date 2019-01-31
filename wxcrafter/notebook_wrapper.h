#ifndef NOTEBOOKWRAPPER_H
#define NOTEBOOKWRAPPER_H

#include "notebook_base_wrapper.h" // Base class: NotebookBaseWrapper

class NotebookWrapper : public NotebookBaseWrapper
{
public:
    NotebookWrapper();
    virtual ~NotebookWrapper();

    wxcWidget* Clone() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    wxString GetXRCPageClass() const;
};

#endif // NOTEBOOKWRAPPER_H
