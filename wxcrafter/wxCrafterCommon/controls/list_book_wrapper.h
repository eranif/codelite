#ifndef LISTBOOKWRAPPER_H
#define LISTBOOKWRAPPER_H

#include "Containers/notebook_base_wrapper.h" // Base class: NotebookBaseWrapper

class ListBookWrapper : public NotebookBaseWrapper
{
public:
    ListBookWrapper();
    ~ListBookWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    wxString GetXRCPageClass() const override;
};

#endif // LISTBOOKWRAPPER_H
