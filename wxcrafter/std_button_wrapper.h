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
    virtual ~StdButtonWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // STDBUTTONWRAPPER_H
