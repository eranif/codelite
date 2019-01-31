#ifndef MYCOMBOBOXXMLHANDLER_H
#define MYCOMBOBOXXMLHANDLER_H

#include <wx/arrstr.h>
#include <wx/xrc/xmlreshandler.h> // Base class: wxXmlResourceHandler

class wxXmlNode;
class MyComboBoxXmlHandler : public wxXmlResourceHandler
{
private:
    bool m_insideBox;
    wxArrayString strList;

public:
    MyComboBoxXmlHandler();
    virtual ~MyComboBoxXmlHandler();
    wxObject* DoCreateResource();
    bool CanHandle(wxXmlNode* node);
};

#endif // MYCOMBOBOXXMLHANDLER_H
