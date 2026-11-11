#pragma once

#include <wx/xrc/xmlreshandler.h>

class wxSimplebook;

class MyWxSimplebookXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxSimplebookXmlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;

private:
    bool m_isInside = false;
    wxSimplebook* m_notebook = nullptr;
};
