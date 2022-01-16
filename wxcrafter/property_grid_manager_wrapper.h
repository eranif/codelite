#ifndef PROPERTYGRIDMANAGERWRAPPER_H
#define PROPERTYGRIDMANAGERWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class PropertyGridManagerWrapper : public wxcWidget
{
public:
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual wxString DoGenerateCppCtorCode_End() const;
    PropertyGridManagerWrapper();
    virtual ~PropertyGridManagerWrapper();
};

#endif // PROPERTYGRIDMANAGERWRAPPER_H
