#ifndef BITMAPBUTTON_H
#define BITMAPBUTTON_H

#include "wxc_widget.h" // Base class: WrapperBase

class BitmapButtonWrapper : public wxcWidget
{
protected:
    void GetIncludeFile(wxArrayString& headers) const override;

public:
    BitmapButtonWrapper();
    ~BitmapButtonWrapper() override = default;

public:
    wxString GetWxClassName() const override;
    wxString CppCtorCode() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;

    wxcWidget* Clone() const override { return new BitmapButtonWrapper(); }
};

#endif // BITMAPBUTTON_H
