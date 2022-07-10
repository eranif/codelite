#include "DAPTerminalCtrlView.h"

#include "StringUtils.h"
#include "globals.h"
#include "macros.h"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

DAPTerminalCtrlView::DAPTerminalCtrlView(wxWindow* parent)
    : DAPOutputViewBase(parent)
{
    m_ctrl = new clTerminalViewCtrl(this);
    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    m_ctrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &DAPTerminalCtrlView::OnMenu, this);
}

DAPTerminalCtrlView::~DAPTerminalCtrlView() {}

void DAPTerminalCtrlView::OnMenu(wxDataViewEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("clear_view_content"), _("Clear"));
    menu.Append(XRCID("copy_view_content"), _("Copy"));

    menu.Bind(
        wxEVT_UPDATE_UI,
        [this](wxUpdateUIEvent& e) {
            // if we have a selection, enable the copy
            e.Enable(m_ctrl->GetSelectedItemsCount() > 0);
        },
        XRCID("copy_view_content"));

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& e) {
            wxUnusedVar(e);
            wxString content;
            wxDataViewItemArray selections;
            m_ctrl->GetSelections(selections);

            if(selections.empty()) {
                return;
            }

            for(size_t i = 0; i < selections.size(); ++i) {
                wxString line = m_ctrl->GetItemText(selections[i]);
                StringUtils::StripTerminalColouring(line, line);
                content << line << "\n";
            }
            content.RemoveLast();
            ::CopyToClipboard(content);
        },
        XRCID("copy_view_content"));
    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& e) {
            wxUnusedVar(e);
            m_ctrl->DeleteAllItems();
        },
        XRCID("clear_view_content"));

    m_ctrl->PopupMenu(&menu);
}