#ifndef TEXTCTRLWRAPPER_H
#define TEXTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class TextCtrlWrapper : public wxcWidget
{

public:
    TextCtrlWrapper();
    ~TextCtrlWrapper() override = default;

public:
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    wxString CppCtorCode() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxcWidget* Clone() const override;
};

#endif // TEXTCTRLWRAPPER_H
