#ifndef PROPERTYGRIDWRAPPER_H
#define PROPERTYGRIDWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class PropertyGridWrapper : public wxcWidget
{
public:
    virtual bool IsValidParent() const;
    virtual bool IsWxWindow() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual wxString DoGenerateCppCtorCode_End() const;
    PropertyGridWrapper();
    virtual ~PropertyGridWrapper();
};

#endif // PROPERTYGRIDWRAPPER_H
