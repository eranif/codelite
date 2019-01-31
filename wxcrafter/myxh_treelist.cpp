#include "myxh_treelist.h"
#include "designer_panel.h"
#include "wxgui_helpers.h"
#include "wxguicraft_main_view.h"
#include <wx/imaglist.h>
#include <wx/treelist.h>

MyTreeListCtrl::MyTreeListCtrl()
    : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxTL_SINGLE);
    XRC_ADD_STYLE(wxTL_MULTIPLE);
    XRC_ADD_STYLE(wxTL_CHECKBOX);
    XRC_ADD_STYLE(wxTL_3STATE);
    XRC_ADD_STYLE(wxTL_USER_3STATE);
    XRC_ADD_STYLE(wxTL_DEFAULT_STYLE);
    AddWindowStyles();
}

bool MyTreeListCtrl::CanHandle(wxXmlNode* node)
{
    return IsOfClass(node, "wxTreeListCtrl") || IsOfClass(node, "wxTreeListCtrlCol");
}

wxObject* MyTreeListCtrl::DoCreateResource()
{
    if(m_class == "wxTreeListCtrlCol") {
        HandleListCol();

    } else {
        wxASSERT_MSG(m_class == "wxTreeListCtrl", wxT("can't handle unknown node"));

        return HandleListCtrl();
    }
    return m_parentAsWindow;
}

void MyTreeListCtrl::HandleListCol()
{
    // Add column to the wxDataViewListCtrl
    wxTreeListCtrl* const list = wxDynamicCast(m_parentAsWindow, wxTreeListCtrl);
    wxCHECK_RET(list, wxT("must have wxTreeListCtrl parent"));

    // Column properties
    int colwidth = GetLong("width", wxCOL_WIDTH_DEFAULT);
    wxString label = GetText("label");
    wxString salign = GetText("align", false);
    wxString col_flags = GetText("flags", false);

    wxAlignment al = (wxAlignment)wxCrafter::ToAligment(salign);
    int flags = wxCrafter::ColumnFlagsFromString(col_flags);
    list->AppendColumn(label, colwidth, al, flags);
}

wxTreeListCtrl* MyTreeListCtrl::HandleListCtrl()
{
    XRC_MAKE_INSTANCE(list, wxTreeListCtrl)

    list->Create(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle(), GetName());
    CreateChildrenPrivately(list);
    SetupWindow(list);

    // This is a one ugly hack: connect our newly created list control
    // to the designer panel
    // list->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(DesignerPanel::OnTreeListCtrlFocus), NULL,
    // GUICraftMainPanel::m_MainPanel->GetDesignerPanel());
    return list;
}
