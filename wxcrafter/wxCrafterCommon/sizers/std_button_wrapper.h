#ifndef STDBUTTONWRAPPER_H
#define STDBUTTONWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

/**
 * @class StdButtonWrapper
 * @brief a button that can be placed inside wxStdButtonDialogSizer
 */
class StdButtonWrapper : public wxcWidget
{
public:
    StdButtonWrapper();
    ~StdButtonWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // STDBUTTONWRAPPER_H
