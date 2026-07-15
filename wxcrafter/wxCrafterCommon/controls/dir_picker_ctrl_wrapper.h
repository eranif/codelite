#ifndef DIRPICKERCTRLWRAPPER_H
#define DIRPICKERCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class DirPickerCtrlWrapper : public wxcWidget
{
public:
    DirPickerCtrlWrapper();
    ~DirPickerCtrlWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // DIRPICKERCTRLWRAPPER_H
