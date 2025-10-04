#ifndef LISTCTRLCOLUMNWRAPPER_H
#define LISTCTRLCOLUMNWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class ListCtrlColumnWrapper : public wxcWidget
{
public:
    ListCtrlColumnWrapper();
    ~ListCtrlColumnWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    /**
     * @brief return the "real" name as an empty string. This will allow use placing
     * more than one column with a similar name in the designer
     */
    wxString GetRealName() const override { return ""; }
};

#endif // LISTCTRLCOLUMNWRAPPER_H
