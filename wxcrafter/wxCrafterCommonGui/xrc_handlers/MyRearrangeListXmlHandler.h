#pragma once

#include <wx/arrstr.h>
#include <wx/xrc/xmlreshandler.h>

class wxXmlNode;
class MyRearrangeListXmlHandler : public wxXmlResourceHandler
{
private:
    bool m_insideBox = false;
    wxArrayString strList;

public:
    MyRearrangeListXmlHandler();
    ~MyRearrangeListXmlHandler() override = default;

    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};
