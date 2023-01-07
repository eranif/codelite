#include "MyComboBoxXmlHandler.h"
#include <wx/combobox.h>
#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>

MyComboBoxXmlHandler::MyComboBoxXmlHandler()
    : wxXmlResourceHandler()
    , m_insideBox(false)
{
    XRC_ADD_STYLE(wxCB_SIMPLE);
    XRC_ADD_STYLE(wxCB_SORT);
    XRC_ADD_STYLE(wxCB_READONLY);
    XRC_ADD_STYLE(wxCB_DROPDOWN);
    XRC_ADD_STYLE(wxTE_PROCESS_ENTER);
    AddWindowStyles();
}

MyComboBoxXmlHandler::~MyComboBoxXmlHandler() {}

wxObject* MyComboBoxXmlHandler::DoCreateResource()
{
    if(m_class == wxT("wxComboBox")) {
        // find the selection
        long selection = GetLong(wxT("selection"), -1);

        // need to build the list of strings from children
        m_insideBox = true;
        CreateChildrenPrivately(NULL, GetParamNode(wxT("content")));

        XRC_MAKE_INSTANCE(control, wxComboBox)

        control->Create(m_parentAsWindow, GetID(), GetText(wxT("value")), GetPosition(), GetSize(), strList, GetStyle(),
                        wxDefaultValidator, GetName());

        if(selection != -1) control->SetSelection(selection);

        if(HasParam("hint")) { control->SetHint(GetText("hint")); }

        SetupWindow(control);

        strList.Clear(); // dump the strings

        return control;
    } else {
        // on the inside now.
        // handle <item>Label</item>

        // add to the list
        wxString str = GetNodeContent(m_node);
        if(m_resource->GetFlags() & wxXRC_USE_LOCALE) str = wxGetTranslation(str, m_resource->GetDomain());
        strList.Add(str);

        return NULL;
    }
}

bool MyComboBoxXmlHandler::CanHandle(wxXmlNode* node)
{
    return (IsOfClass(node, wxT("wxComboBox")) || (m_insideBox && node->GetName() == wxT("item")));
}
