#include "MyRearrangeListXmlHandler.h"
#include <wx/defs.h>
#include <wx/rearrangectrl.h>
#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>

MyRearrangeListXmlHandler::MyRearrangeListXmlHandler()
    : wxXmlResourceHandler()
    , m_insideBox(false)
{
    // wxListBox styles:
    XRC_ADD_STYLE(wxLB_SINGLE);
    XRC_ADD_STYLE(wxLB_MULTIPLE);
    XRC_ADD_STYLE(wxLB_EXTENDED);
    XRC_ADD_STYLE(wxLB_HSCROLL);
    XRC_ADD_STYLE(wxLB_ALWAYS_SB);
    XRC_ADD_STYLE(wxLB_NEEDED_SB);
    XRC_ADD_STYLE(wxLB_SORT);
    AddWindowStyles();
}

MyRearrangeListXmlHandler::~MyRearrangeListXmlHandler() {}

bool MyRearrangeListXmlHandler::CanHandle(wxXmlNode* node)
{
    return (IsOfClass(node, wxT("wxRearrangeList")) || (m_insideBox && node->GetName() == wxT("item")));
}

wxObject* MyRearrangeListXmlHandler::DoCreateResource()
{
    if(m_class == wxT("wxRearrangeList")) {
        // need to build the list of strings from children
        m_insideBox = true;
        CreateChildrenPrivately(NULL, GetParamNode(wxT("content")));

        wxArrayInt order;
        for(size_t i = 0; i < strList.GetCount(); ++i) {
            order.push_back((int)i);
        }

        XRC_MAKE_INSTANCE(control, wxRearrangeList)
        control->Create(m_parentAsWindow, GetID(), GetPosition(), GetSize(), order, strList, GetStyle(),
                        wxDefaultValidator, GetName());

        // step through children myself (again.)
        wxXmlNode* n = GetParamNode(wxT("content"));
        if(n) n = n->GetChildren();
        int i = 0;
        while(n) {
            if(n->GetType() != wxXML_ELEMENT_NODE || n->GetName() != wxT("item")) {
                n = n->GetNext();
                continue;
            }

            // checking boolean is a bit ugly here (see GetBool() )
            wxString v = n->GetAttribute(wxT("checked"), wxEmptyString);
            v.MakeLower();
            if(v == wxT("1")) control->Check(i, true);

            i++;
            n = n->GetNext();
        }

        SetupWindow(control);
        strList.Clear(); // dump the strings
        return control;

    } else {
        // on the inside now.
        // handle <item checked="boolean">Label</item>

        // add to the list
        wxString str = GetNodeContent(m_node);
        if(m_resource->GetFlags() & wxXRC_USE_LOCALE) str = wxGetTranslation(str, m_resource->GetDomain());
        strList.Add(str);
        return NULL;
    }
}
