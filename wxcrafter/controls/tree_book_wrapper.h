#ifndef TREEBOOKWRAPPER_H
#define TREEBOOKWRAPPER_H

#include "notebook_base_wrapper.h" // Base class: NotebookBaseWrapper

class TreeBookWrapper : public NotebookBaseWrapper
{
public:
    TreeBookWrapper();
    ~TreeBookWrapper() override = default;

public:
    wxString GetXRCPageClass() const override;
    wxcWidget* Clone() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
};

#endif // TREEBOOKWRAPPER_H
