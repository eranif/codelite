#ifndef STDDIALOGBUTTONSIZERWRAPPER_H
#define STDDIALOGBUTTONSIZERWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase
#include <wx/event.h>

class StdDialogButtonSizerWrapper : public wxEvtHandler, public wxcWidget
{
public:
    StdDialogButtonSizerWrapper();
    ~StdDialogButtonSizerWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;

    bool IsValidParent() const override { return false; }
};

#endif // STDDIALOGBUTTONSIZERWRAPPER_H
