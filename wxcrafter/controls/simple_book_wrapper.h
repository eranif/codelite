#ifndef SIMPLEBOOKWRAPPER_H
#define SIMPLEBOOKWRAPPER_H

#include "notebook_base_wrapper.h" // Base class: NotebookBaseWrapper

class SimpleBookWrapper : public NotebookBaseWrapper
{
public:
    SimpleBookWrapper();
    ~SimpleBookWrapper() override = default;

public:
    wxString GetXRCPageClass() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxcWidget* Clone() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    wxString CppCtorCode() const override;
};

#endif // SIMPLEBOOKWRAPPER_H
