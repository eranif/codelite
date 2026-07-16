#ifndef NOTEBOOKWRAPPER_H
#define NOTEBOOKWRAPPER_H

#include "notebook_base_wrapper.h" // Base class: NotebookBaseWrapper

class NotebookWrapper : public NotebookBaseWrapper
{
public:
    NotebookWrapper();
    ~NotebookWrapper() override = default;

    wxcWidget* Clone() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    wxString GetXRCPageClass() const override;
};

#endif // NOTEBOOKWRAPPER_H
