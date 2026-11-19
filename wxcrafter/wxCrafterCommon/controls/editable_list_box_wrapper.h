#pragma once

#include "wxc_widget.h"

class EditableListBoxWrapper : public wxcWidget
{
public:
    EditableListBoxWrapper();
    ~EditableListBoxWrapper() override = default;

    wxcWidget* Clone() const override { return new EditableListBoxWrapper(); }
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
};
