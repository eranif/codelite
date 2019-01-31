#include "myxh_propgrid.h"
#include <wx/datetime.h>
#include <wx/log.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/propgrid.h>
#include <wx/sizer.h>
#include <wx/xml/xml.h>

MyWxPropGridXmlHandler::MyWxPropGridXmlHandler()
    : wxXmlResourceHandler()
    , m_isInside(false)
    , m_pgmgr(NULL)
    , m_propertyParent(NULL)
{
    XRC_ADD_STYLE(wxPG_AUTO_SORT);
    XRC_ADD_STYLE(wxPG_HIDE_CATEGORIES);
    XRC_ADD_STYLE(wxPG_ALPHABETIC_MODE);
    XRC_ADD_STYLE(wxPG_BOLD_MODIFIED);
    XRC_ADD_STYLE(wxPG_SPLITTER_AUTO_CENTER);
    XRC_ADD_STYLE(wxPG_TOOLTIPS);
    XRC_ADD_STYLE(wxPG_HIDE_MARGIN);
    XRC_ADD_STYLE(wxPG_STATIC_SPLITTER);
    XRC_ADD_STYLE(wxPG_STATIC_LAYOUT);
    XRC_ADD_STYLE(wxPG_LIMITED_EDITING);
    XRC_ADD_STYLE(wxPG_TOOLBAR);
    XRC_ADD_STYLE(wxPG_DESCRIPTION);
    XRC_ADD_STYLE(wxPG_NO_INTERNAL_BORDER);
    AddWindowStyles();
}

wxObject* MyWxPropGridXmlHandler::DoCreateResource()
{
    if(m_class == wxT("wxPGProperty")) {
        HandlePgProperty(m_propertyParent);

    } else {
        // wxPropertyGridManager
        XRC_MAKE_INSTANCE(pg, wxPropertyGridManager);
        pg->Create(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle(), GetName());
        SetupWindow(pg);

        m_pgmgr = pg;
        m_pgmgr->AddPage("Settings");
        m_propertyParent = NULL;
        m_isInside = true;
        CreateChildren(m_pgmgr, true /*only this handler*/);

        // add the splitter property after the children were added
        int splitterLeft = GetBool("splitterleft");
        if(splitterLeft) { m_pgmgr->GetPage(0)->SetSplitterLeft(); }

        int splitterPos = GetLong("splitterpos", wxNOT_FOUND);
        if(splitterPos != wxNOT_FOUND) { m_pgmgr->GetPage(0)->SetSplitterPosition(splitterPos); }

        m_isInside = false;
        return m_pgmgr;
    }
    return m_parentAsWindow;
}

bool MyWxPropGridXmlHandler::CanHandle(wxXmlNode* node)
{
    return ((!m_isInside && IsOfClass(node, wxT("wxPropertyGridManager"))) ||
            (m_isInside && IsOfClass(node, wxT("wxPGProperty"))));
}

void MyWxPropGridXmlHandler::HandlePgProperty(wxPGProperty* parent)
{
    if(!HasParam(wxT("proptype"))) { return; }

    // Property
    wxString proptype = GetText("proptype");
    wxString label = GetText("label");
    wxString value = GetText("value");
    wxString wildcard = GetText("wildcard", false);
    wxString tip = GetText("tooltip");
    wxColour bgcol = GetColour("bg");
    bool bgcolRecurse = GetBool("bg-recurse");
    wxString propEditor = GetText("editor", false);

    wxArrayString items = GetArray(GetParamNode("content"));

    wxPGProperty* prop = NULL;
    if(proptype == "wxIntProperty") {
        long v;
        value.ToLong(&v);
        prop = DoAppendProperty(parent, new wxIntProperty(label, wxPG_LABEL, v));

    } else if(proptype == "wxFloatProperty") {
        double v;
        value.ToDouble(&v);
        prop = DoAppendProperty(parent, new wxFloatProperty(label, wxPG_LABEL, v));

    } else if(proptype == "wxBoolProperty") {
        prop = DoAppendProperty(parent, new wxBoolProperty(label, wxPG_LABEL, value == "1"));

    } else if(proptype == "wxStringProperty") {
        prop = DoAppendProperty(parent, new wxStringProperty(label, wxPG_LABEL, value));

    } else if(proptype == "wxLongStringProperty") {
        prop = DoAppendProperty(parent, new wxLongStringProperty(label, wxPG_LABEL, value));

    } else if(proptype == "wxDirProperty") {
        prop = DoAppendProperty(parent, new wxDirProperty(label, wxPG_LABEL, value));

    } else if(proptype == "wxArrayStringProperty") {
        prop = DoAppendProperty(parent, new wxArrayStringProperty(label, wxPG_LABEL, items));

    } else if(proptype == "wxFileProperty") {
        prop = DoAppendProperty(parent, new wxFileProperty(label, wxPG_LABEL, value));
        m_pgmgr->SetPropertyAttribute(prop, wxPG_FILE_WILDCARD, wildcard);

    } else if(proptype == "wxPropertyCategory") {
        prop = DoAppendProperty(parent, new wxPropertyCategory(label));

    } else if(proptype == "wxEnumProperty") {
        int sel = items.Index(value);
        if(sel == wxNOT_FOUND) { sel = 0; }

        prop = DoAppendProperty(parent, new wxEnumProperty(label, wxPG_LABEL, items, wxArrayInt(), sel));

    } else if(proptype == "wxEditEnumProperty") {
        prop = DoAppendProperty(parent, new wxEditEnumProperty(label, wxPG_LABEL, items, wxArrayInt(), value));

    } else if(proptype == "wxFlagsProperty") {
        prop = DoAppendProperty(parent, new wxFlagsProperty(label, wxPG_LABEL, items, wxArrayInt(), 0));

    } else if(proptype == "wxDateProperty") {
        prop = DoAppendProperty(parent, new wxDateProperty(label, wxPG_LABEL, wxDateTime::Now()));

    } else if(proptype == "wxImageFileProperty") {
        prop = DoAppendProperty(parent, new wxImageFileProperty(label));

    } else if(proptype == "wxFontProperty") {
        prop = DoAppendProperty(parent, new wxFontProperty(label));
        prop->SetValueToUnspecified();

    } else if(proptype == "wxSystemColourProperty") {
        prop = DoAppendProperty(parent, new wxSystemColourProperty(label));
        prop->SetValueToUnspecified();
    }

    // Handle nested children
    if(prop) {
        if(bgcol.IsOk()) { prop->SetBackgroundColour(bgcol, bgcolRecurse); }
        if(!propEditor.IsEmpty()) { prop->SetEditor(propEditor); }
        prop->SetHelpString(tip);

        // Check to see if this property has children
        // But keep the current parent (incase the current parent has more children...)
        wxPGProperty* oldParent = m_propertyParent;
        m_propertyParent = prop;
        CreateChildren(prop, true);

        // Restore the old parent
        m_propertyParent = oldParent;
    }
}

wxArrayString MyWxPropGridXmlHandler::GetArray(const wxXmlNode* node) const
{
    if(!node) return wxArrayString();

    wxArrayString items;
    wxXmlNode* child = node->GetChildren();
    while(child) {
        if(child->GetName() == "item") { items.Add(child->GetNodeContent()); }
        child = child->GetNext();
    }
    return items;
}

wxPGProperty* MyWxPropGridXmlHandler::DoAppendProperty(wxPGProperty* parent, wxPGProperty* prop)
{
    if(parent) {
        return m_pgmgr->GetPage(0)->AppendIn(parent, prop);
    } else {
        return m_pgmgr->GetPage(0)->Append(prop);
    }
}
