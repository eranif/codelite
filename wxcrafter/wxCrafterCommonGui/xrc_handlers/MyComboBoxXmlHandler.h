#pragma once

#include <wx/arrstr.h>
#include <wx/xrc/xmlreshandler.h> // Base class: wxXmlResourceHandler

class MyComboBoxXmlHandler : public wxXmlResourceHandler
{
private:
    bool m_insideBox = false;
    wxArrayString strList;

public:
    MyComboBoxXmlHandler();
    ~MyComboBoxXmlHandler() override = default;

    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};
