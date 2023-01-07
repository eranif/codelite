/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_toolb.cpp
// Purpose:     XRC resource for wxAuiToolBar
// Author:      Vaclav Slavik
// Created:     2000/08/11
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "myxh_auitoolb.h"
#include "wx/aui/auibar.h"
#include "wx/frame.h"
#include "wx/log.h"
#include "wx/menu.h"
#include "wx/pen.h"
#include "wx/sizer.h"
#include "wx/wxprec.h"
#include <wx/xml/xml.h>

MyWxAuiToolBarXmlHandler::MyWxAuiToolBarXmlHandler()
    : wxXmlResourceHandler()
    , m_isInside(false)
    , m_toolbar(NULL)
{
    XRC_ADD_STYLE(wxAUI_TB_TEXT);
    XRC_ADD_STYLE(wxAUI_TB_NO_TOOLTIPS);
    XRC_ADD_STYLE(wxAUI_TB_NO_AUTORESIZE);
    XRC_ADD_STYLE(wxAUI_TB_GRIPPER);
    XRC_ADD_STYLE(wxAUI_TB_OVERFLOW);
    XRC_ADD_STYLE(wxAUI_TB_VERTICAL);
    XRC_ADD_STYLE(wxAUI_TB_HORZ_LAYOUT);
    XRC_ADD_STYLE(wxAUI_TB_HORZ_TEXT);
    XRC_ADD_STYLE(wxAUI_TB_DEFAULT_STYLE);
    XRC_ADD_STYLE(wxAUI_TB_PLAIN_BACKGROUND);
    AddWindowStyles();
}

wxObject* MyWxAuiToolBarXmlHandler::DoCreateResource()
{
    if(m_class == wxT("tool")) {
        if(!m_toolbar) { return NULL; }

        wxItemKind kind = wxITEM_NORMAL;
        if(GetBool(wxT("radio"))) kind = wxITEM_RADIO;

        if(GetBool(wxT("toggle"))) { kind = wxITEM_CHECK; }

        m_toolbar->AddTool(GetID(), GetText(wxT("label")), GetBitmap(wxT("bitmap"), wxART_TOOLBAR, m_toolSize),
                           GetText(wxT("longhelp")), kind);

        if(GetBool(wxT("disabled"))) m_toolbar->EnableTool(GetID(), false);

        if(GetBool(wxS("checked"))) { m_toolbar->ToggleTool(GetID(), true); }

        wxAuiToolBarItem* tool = m_toolbar->FindTool(GetID());
        wxXmlNode* const nodeDropdown = GetParamNode("dropdown"); // The new official wx xh_auitoolb.cpp uses "dropdown"
        if(tool && (GetBool("hasdropdown") || (nodeDropdown != NULL))) {
            // From the official wx3.1 handler
            // also check for the menu specified inside dropdown (it is
            // optional and may be absent for e.g. dynamically-created menus)
            wxMenu* menu = NULL; // menu for drop down items
            wxXmlNode* const nodeMenu = nodeDropdown->GetChildren();
            if(nodeMenu) {
                wxObject* res = CreateResFromNode(nodeMenu, NULL);
                menu = wxDynamicCast(res, wxMenu);
            }
            tool->SetHasDropDown(true);
            if(menu) { tool->SetUserData(m_menuHandler.RegisterMenu(m_toolbar, GetID(), menu)); }
        }
        return m_toolbar; // must return non-NULL
    }

    else if(m_class == wxS("label")) {
        if(m_toolbar) { m_toolbar->AddLabel(GetID(), GetText(wxS("label")), GetLong(wxS("width"), -1l)); }

        return m_toolbar;
    }

    else if(m_class == wxS("space")) {
        if(m_toolbar) {
            // This may be a stretch spacer (the default) or a non-stretch one which will have the attribute 'width'
            if(HasParam("width")) {
                m_toolbar->AddSpacer(GetLong("width"));
            } else {
                m_toolbar->AddStretchSpacer(GetLong(wxS("proportion"), 1l));
            }
        }

        return m_toolbar;
    }

    else if(m_class == wxT("separator")) {
        if(m_toolbar) { m_toolbar->AddSeparator(); }

        return m_toolbar;
    }

    else { /*<object class="wxAuiToolBar">*/
        int style = GetStyle(wxT("style"), wxAUI_TB_DEFAULT_STYLE);
        wxAuiToolBar* toolbar = new wxAuiToolBar(m_parentAsWindow, GetID(), GetPosition(), GetSize(), style);
        toolbar->SetName(GetName());
        SetupWindow(toolbar);

        m_toolSize = GetSize(wxT("bitmapsize"));
        if(!(m_toolSize == wxDefaultSize)) toolbar->SetToolBitmapSize(m_toolSize);
        wxSize margins = GetSize(wxT("margins"));
        if(!(margins == wxDefaultSize)) toolbar->SetMargins(margins.x, margins.y);
        long packing = GetLong(wxT("packing"), -1);
        if(packing != -1) toolbar->SetToolPacking(packing);
        long separation = GetLong(wxT("separation"), -1);
        if(separation != -1) toolbar->SetToolSeparation(separation);

        wxXmlNode* children_node = GetParamNode(wxT("object"));
        if(!children_node) children_node = GetParamNode(wxT("object_ref"));

        if(children_node == NULL) return toolbar;

        m_isInside = true;
        m_toolbar = toolbar;

        wxXmlNode* n = children_node;

        while(n) {
            if((n->GetType() == wxXML_ELEMENT_NODE) &&
               (n->GetName() == wxT("object") || n->GetName() == wxT("object_ref"))) {
                wxObject* created = CreateResFromNode(n, toolbar, NULL);
                wxControl* control = wxDynamicCast(created, wxControl);
                if(!IsOfClass(n, wxT("tool")) && !IsOfClass(n, wxT("label")) && !IsOfClass(n, wxT("separator")) &&
                   !IsOfClass(n, wxT("space")) && // == AddStretchSpacer()
                   control != NULL)
                    toolbar->AddControl(control);
            }
            n = n->GetNext();
        }

        m_isInside = false;
        m_toolbar = NULL;

        toolbar->Realize();
        return toolbar;
    }
}

bool MyWxAuiToolBarXmlHandler::CanHandle(wxXmlNode* node)
{
    return ((!m_isInside && IsOfClass(node, wxT("wxAuiToolBar"))) || (m_isInside && IsOfClass(node, wxT("tool"))) ||
            (m_isInside && IsOfClass(node, wxT("label"))) || (m_isInside && IsOfClass(node, wxT("space"))) ||
            (m_isInside && IsOfClass(node, wxT("separator"))));
}

// -------------------------- From the official wx3.1 handler ----------------------
void MyWxAuiToolBarXmlHandler::MenuHandler::OnDropDown(wxAuiToolBarEvent& event)
{
    if(event.IsDropDownClicked()) {
        wxAuiToolBar* toobar = wxDynamicCast(event.GetEventObject(), wxAuiToolBar);
        if(toobar != NULL) {
            wxAuiToolBarItem* item = toobar->FindTool(event.GetId());
            if(item != NULL) {
                wxMenu* const menu = m_menus[item->GetUserData()];
                if(menu != NULL) {
                    wxPoint pt = event.GetItemRect().GetBottomLeft();
                    pt.y++;
                    toobar->PopupMenu(menu, pt);
                }
            }
        }
    } else {
        event.Skip();
    }
}

unsigned MyWxAuiToolBarXmlHandler::MenuHandler::RegisterMenu(wxAuiToolBar* toolbar, int id, wxMenu* menu)
{
    m_menus.push_back(menu);
    toolbar->Bind(wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN, &MyWxAuiToolBarXmlHandler::MenuHandler::OnDropDown, this, id);

    return m_menus.size() - 1;
}
