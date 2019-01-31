#ifndef AUINOTEBOOKWRAPPER_H
#define AUINOTEBOOKWRAPPER_H

#include "notebook_base_wrapper.h" // Base class: NotebookBaseWrapper

class AuiNotebookWrapper : public NotebookBaseWrapper
{
public:
    AuiNotebookWrapper();
    virtual ~AuiNotebookWrapper();

public:
    virtual wxString GetXRCPageClass() const;
    wxcWidget* Clone() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
};

#endif // AUINOTEBOOKWRAPPER_H
