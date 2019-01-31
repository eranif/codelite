#ifndef MYREARRANGELISTXMLHANDLER_H
#define MYREARRANGELISTXMLHANDLER_H

#include <wx/arrstr.h>
#include <wx/xrc/xmlreshandler.h>

class wxXmlNode;
class MyRearrangeListXmlHandler : public wxXmlResourceHandler
{
private:
    bool m_insideBox;
    wxArrayString strList;

public:
    MyRearrangeListXmlHandler();
    virtual ~MyRearrangeListXmlHandler();
    wxObject* DoCreateResource();
    bool CanHandle(wxXmlNode* node);
};
#endif // MYREARRANGELISTXMLHANDLER_H
