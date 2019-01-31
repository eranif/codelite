#ifndef TREEBOOKWRAPPER_H
#define TREEBOOKWRAPPER_H

#include "notebook_base_wrapper.h" // Base class: NotebookBaseWrapper

class TreeBookWrapper : public NotebookBaseWrapper
{
public:
    TreeBookWrapper();
    virtual ~TreeBookWrapper();

public:
    virtual wxString GetXRCPageClass() const;
    wxcWidget* Clone() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
};

#endif // TREEBOOKWRAPPER_H
