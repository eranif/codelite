#ifndef AUIMANAGERWRAPPER_H
#define AUIMANAGERWRAPPER_H

#include <wx/bitmap.h>
#include "wxc_widget.h" // Base class: wxcWidget

class AuiManagerWrapper : public wxcWidget
{
public:
    AuiManagerWrapper();
    virtual ~AuiManagerWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual wxString CppDtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual bool IsValidParent() const { return false; }
    virtual bool IsWxWindow() const { return false; }
};

#endif // AUIMANAGERWRAPPER_H
