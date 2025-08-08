#ifndef AUINOTEBOOKWRAPPER_H
#define AUINOTEBOOKWRAPPER_H

#include "controls/Containers/notebook_base_wrapper.h" // Base class: NotebookBaseWrapper

class AuiNotebookWrapper : public NotebookBaseWrapper
{
public:
    AuiNotebookWrapper();
    ~AuiNotebookWrapper() override = default;

public:
    wxString GetXRCPageClass() const override;
    wxcWidget* Clone() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
};

#endif // AUINOTEBOOKWRAPPER_H
