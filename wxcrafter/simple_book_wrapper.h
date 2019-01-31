#ifndef SIMPLEBOOKWRAPPER_H
#define SIMPLEBOOKWRAPPER_H

#include "notebook_base_wrapper.h" // Base class: NotebookBaseWrapper

class SimpleBookWrapper : public NotebookBaseWrapper
{
public:
    SimpleBookWrapper();
    virtual ~SimpleBookWrapper();

public:
    virtual wxString GetXRCPageClass() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    wxcWidget* Clone() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    virtual wxString CppCtorCode() const;
};

#endif // SIMPLEBOOKWRAPPER_H
