#ifndef LISTBOOKWRAPPER_H
#define LISTBOOKWRAPPER_H

#include "notebook_base_wrapper.h" // Base class: NotebookBaseWrapper

class ListBookWrapper : public NotebookBaseWrapper
{
public:
    ListBookWrapper();
    virtual ~ListBookWrapper();

public:
    wxcWidget* Clone() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    wxString GetXRCPageClass() const;
};

#endif // LISTBOOKWRAPPER_H
