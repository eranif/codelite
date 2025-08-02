#ifndef TOOLBOOKWRAPPER_H
#define TOOLBOOKWRAPPER_H

#include "notebook_base_wrapper.h" // Base class: NotebookBaseWrapper

class ToolBookWrapper : public NotebookBaseWrapper
{
public:
    ToolBookWrapper();
    ~ToolBookWrapper() override = default;

    wxcWidget* Clone() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    wxString GetXRCPageClass() const override;
};

#endif // TOOLBOOKWRAPPER_H
