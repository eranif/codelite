#include "LanguageServerLogView.h"

#include "JSON.h"
#include "LanguageServerCluster.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"

namespace
{
struct MyTreeData : public wxTreeItemData {
    LSP::Command m_command;
    wxString m_filepath;

    MyTreeData(const LSP::Command& command, const wxString& filepath)
        : m_command(command)
        , m_filepath(filepath)
    {
    }
};
} // namespace

LanguageServerLogView::LanguageServerLogView(wxWindow* parent, LanguageServerCluster* cluster)
    : LanguageServerLogViewBase(parent)
    , m_cluster(cluster)
{
    m_dvListCtrl->Bind(wxEVT_CONTEXT_MENU, [this](wxContextMenuEvent& event) {
        wxMenu menu;
        menu.Append(wxID_CLEAR);
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent& event) {
                wxUnusedVar(event);
                // clear the view
                m_dvListCtrl->DeleteAllItems();
            },
            wxID_CLEAR);
        m_dvListCtrl->PopupMenu(&menu);
    });

    m_treeCtrlProblems->AddRoot(_("Code Actions"), wxNOT_FOUND, wxNOT_FOUND, NULL);
    m_treeCtrlProblems->AddHeader(_("Title"));
    m_treeCtrlProblems->AddHeader(_("- Action Button - "));
    EventNotifier::Get()->Bind(wxEVT_LSP_CODE_ACTIONS, &LanguageServerLogView::OnCodeActions, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_CLEAR_DIAGNOSTICS, &LanguageServerLogView::OnClearActions, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &LanguageServerLogView::OnWorkspaceClosed, this);
}

LanguageServerLogView::~LanguageServerLogView()
{
    EventNotifier::Get()->Unbind(wxEVT_LSP_CODE_ACTIONS, &LanguageServerLogView::OnCodeActions, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_CLEAR_DIAGNOSTICS, &LanguageServerLogView::OnClearActions, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &LanguageServerLogView::OnWorkspaceClosed, this);
}

void LanguageServerLogView::OnCodeActions(LSPEvent& event)
{
    // always skip
    event.Skip();

    const auto& commands = event.GetCommands();
    const auto& filepath = event.GetFileName();
    auto root = m_treeCtrlProblems->GetRootItem();
    auto file_item = FindFile(filepath);

    if(commands.empty()) {
        // delete the file and all its children
        CHECK_ITEM_RET(file_item); // if no file item -> just leave the handler
        m_treeCtrlProblems->Delete(file_item);
    } else {
        m_treeCtrlProblems->Begin();
        if(!file_item.IsOk()) {
            file_item = m_treeCtrlProblems->AppendItem(root, event.GetFileName());
        } else {
            m_treeCtrlProblems->DeleteChildren(file_item);
        }

        for(const LSP::Command& d : commands) {
            // add a button
            wxButton* btn = new wxButton(m_treeCtrlProblems, wxID_ANY, _("Apply fix"));
            btn->Hide();
            auto row = m_treeCtrlProblems->AppendItem(file_item, d.GetTitle(), wxNOT_FOUND, wxNOT_FOUND,
                                                      new MyTreeData(d, filepath));
            m_treeCtrlProblems->SetItemControl(row, btn, 1);

            btn->Bind(wxEVT_BUTTON, [d, filepath, this](wxCommandEvent& event) {
                // apply the fix
                wxUnusedVar(event);
                auto server = m_cluster->GetServerForFile(filepath);
                server->SendWorkspaceExecuteCommand(filepath, d);
            });
        }

        m_treeCtrlProblems->Commit();

        // ensure that the root is expanded
        m_treeCtrlProblems->Expand(root);
    }
}

void LanguageServerLogView::OnClearActions(LSPEvent& event)
{
    // always skip
    event.Skip();
    const wxString& filepath = event.GetFileName();
    auto file_item = FindFile(filepath);
    CHECK_ITEM_RET(file_item);

    m_treeCtrlProblems->Delete(file_item);
}

wxTreeItemId LanguageServerLogView::FindFile(const wxString& filepath) const
{
    auto root = m_treeCtrlProblems->GetRootItem();
    wxTreeItemIdValue cookie;
    auto child = m_treeCtrlProblems->GetFirstChild(root, cookie);
    while(child.IsOk()) {
        if(m_treeCtrlProblems->GetItemText(child) == filepath) {
            return child;
        }

        child = m_treeCtrlProblems->GetNextChild(root, cookie);
    }
    return {};
}

void LanguageServerLogView::OnDiagnosticSelected(wxTreeEvent& event)
{
    event.Skip();
    auto item_data = m_treeCtrlProblems->GetItemData(event.GetItem());
    CHECK_PTR_RET(item_data);

    auto cd = dynamic_cast<MyTreeData*>(item_data);
    CHECK_PTR_RET(cd);
    clGetManager()->OpenFile(cd->m_filepath);
}

void LanguageServerLogView::OnWorkspaceClosed(clWorkspaceEvent& event) { event.Skip(); }