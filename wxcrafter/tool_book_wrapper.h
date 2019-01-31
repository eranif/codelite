#ifndef TOOLBOOKWRAPPER_H
#define TOOLBOOKWRAPPER_H

#include "notebook_base_wrapper.h" // Base class: NotebookBaseWrapper

class ToolBookWrapper : public NotebookBaseWrapper
{
public:
    ToolBookWrapper();
    virtual ~ToolBookWrapper();
    wxcWidget* Clone() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    wxString GetXRCPageClass() const;
};

#endif // TOOLBOOKWRAPPER_H
