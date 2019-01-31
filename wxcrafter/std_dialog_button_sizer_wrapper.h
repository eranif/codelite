#ifndef STDDIALOGBUTTONSIZERWRAPPER_H
#define STDDIALOGBUTTONSIZERWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase
#include <wx/event.h>

class StdDialogButtonSizerWrapper : public wxEvtHandler, public wxcWidget
{
public:
    StdDialogButtonSizerWrapper();
    virtual ~StdDialogButtonSizerWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    void ToXRC(wxString& text, XRC_TYPE type) const;
    void LoadPropertiesFromwxFB(const wxXmlNode* node);

    bool IsValidParent() const { return false; }
};

#endif // STDDIALOGBUTTONSIZERWRAPPER_H
