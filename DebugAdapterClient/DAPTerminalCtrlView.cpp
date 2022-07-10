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
    menu.Append(wxID_CLEAR);
    menu.Append(wxID_COPY);

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
        wxID_COPY);
    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& e) {
            wxUnusedVar(e);
            m_ctrl->DeleteAllItems();
        },
        wxID_CLEAR);

    m_ctrl->PopupMenu(&menu);
}