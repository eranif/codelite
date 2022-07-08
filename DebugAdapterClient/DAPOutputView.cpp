#include "DAPOutputView.h"

#include "StringUtils.h"
#include "globals.h"
#include "macros.h"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

DAPOutputView::DAPOutputView(wxWindow* parent)
    : DAPOutputViewBase(parent)
{
    m_ctrl = new clTerminalViewCtrl(this);
    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    m_ctrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &DAPOutputView::OnMenu, this);
}

DAPOutputView::~DAPOutputView() {}

void DAPOutputView::OnMenu(wxDataViewEvent& event)
{
    wxMenu menu;
    menu.Append(wxID_CLEAR);
    menu.Append(wxID_COPY);

    menu.Bind(
        wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& e) { e.Enable(!m_ctrl->IsEmpty()); }, wxID_CLEAR);
    menu.Bind(
        wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& e) { e.Enable(m_ctrl->GetSelectedItemsCount() > 0); }, wxID_COPY);
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