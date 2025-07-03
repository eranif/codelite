#ifndef SPACERWRAPPER_H
#define SPACERWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class SpacerWrapper : public wxcWidget
{
public:
    SpacerWrapper();
    ~SpacerWrapper() override = default;

    bool IsEventHandler() const override { return false; }
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    bool IsSizer() const override { return true; }
};

#endif // SPACERWRAPPER_H
