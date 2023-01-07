#include "myxh_dvtreectrl.h"
#include <wx/dataview.h>
#include <wx/imaglist.h>

MyWxDataViewTreeCtrlHandler::MyWxDataViewTreeCtrlHandler()
    : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxDV_SINGLE);
    XRC_ADD_STYLE(wxDV_MULTIPLE);
    XRC_ADD_STYLE(wxDV_ROW_LINES);
    XRC_ADD_STYLE(wxDV_HORIZ_RULES);
    XRC_ADD_STYLE(wxDV_VERT_RULES);
    XRC_ADD_STYLE(wxDV_VARIABLE_LINE_HEIGHT);
    XRC_ADD_STYLE(wxDV_NO_HEADER);
    AddWindowStyles();
}

wxObject* MyWxDataViewTreeCtrlHandler::DoCreateResource()
{
    wxASSERT_MSG(m_class == "wxDataViewTreeCtrl", wxT("can't handle unknown node"));

    return HandleListCtrl();
}

bool MyWxDataViewTreeCtrlHandler::CanHandle(wxXmlNode* node) { return IsOfClass(node, "wxDataViewTreeCtrl"); }

wxDataViewTreeCtrl* MyWxDataViewTreeCtrlHandler::HandleListCtrl()
{
    XRC_MAKE_INSTANCE(list, wxDataViewTreeCtrl)

    list->Create(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle());
    list->SetName(GetName());
    CreateChildrenPrivately(list);
    SetupWindow(list);
    return list;
}
