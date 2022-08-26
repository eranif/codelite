#include "DAPWatchesView.h"

#include "DebugAdapterClient.hpp"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "macros.h"

DAPWatchesView::DAPWatchesView(wxWindow* parent, DebugAdapterClient* plugin, clModuleLogger& log)
    : DAPWatchesViewBase(parent)
    , m_plugin(plugin)
    , LOG(log)
{
    m_list = new DAPVariableListCtrl(this, &m_plugin->GetClient(), dap::EvaluateContext::WATCH);
    GetSizer()->Add(m_list, wxSizerFlags(1).Expand());

    clBitmapList* bitmaps = new clBitmapList;
    m_toolbar->AddTool(XRCID("dap-watch-new"), _("New"), bitmaps->Add("file_new"));
    m_toolbar->AddTool(XRCID("dap-watch-delete"), _("Delete"), bitmaps->Add("file_close"));
    m_toolbar->AddStretchableSpace();
    m_toolbar->AddTool(XRCID("dap-watch-delete-all"), _("Delete All"), bitmaps->Add("clear"));
    m_toolbar->AssignBitmaps(bitmaps);
    m_toolbar->Realize();

    m_toolbar->Bind(wxEVT_TOOL, &DAPWatchesView::OnNewWatch, this, XRCID("dap-watch-new"));
    m_toolbar->Bind(wxEVT_TOOL, &DAPWatchesView::OnDeleteWatch, this, XRCID("dap-watch-delete"));
    m_toolbar->Bind(wxEVT_TOOL, &DAPWatchesView::OnDeleteAll, this, XRCID("dap-watch-delete-all"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &DAPWatchesView::OnDeleteAllUI, this, XRCID("dap-watch-delete-all"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &DAPWatchesView::OnDeleteWatchUI, this, XRCID("dap-watch-delete"));
}

DAPWatchesView::~DAPWatchesView() {}

void DAPWatchesView::Update(int current_frame_id)
{
    // update the watches
    m_list->Begin();

    wxTreeItemIdValue cookie;
    auto item = m_list->GetFirstChild(m_list->GetRootItem(), cookie);
    wxArrayString words;

    // two loops, to avoid manipulating the tree while we are adding entries
    while(item.IsOk()) {
        wxString word = m_list->GetItemText(item);
        words.Add(word);
        item = m_list->GetNextChild(m_list->GetRootItem(), cookie);
    }

    m_list->DeleteChildren(m_list->GetRootItem());
    for(const auto& word : words) {
        m_plugin->GetClient().EvaluateExpression(
            word, current_frame_id, dap::EvaluateContext::WATCH,
            [this, word](bool success, const wxString& result, const wxString& type, int variablesReference) {
                if(!success) {
                    m_list->AddWatch(word, wxEmptyString, wxEmptyString, 0);
                } else {
                    m_list->AddWatch(word, result, type, variablesReference);
                }
            });
    }
    m_list->Commit();
}

void DAPWatchesView::OnNewWatch(wxCommandEvent& event)
{
    wxUnusedVar(event);
    auto editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(m_plugin->GetCurrentFrameId() != wxNOT_FOUND);

    wxString selected_text = editor->GetSelection();
    wxString expression = ::clGetTextFromUser(_("Add watch"), _("Expression:"), selected_text);
    if(expression.empty()) {
        return;
    }

    m_list->AppendItem(m_list->GetRootItem(), expression);
    Update(m_plugin->GetCurrentFrameId());
}

void DAPWatchesView::OnDeleteWatch(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    m_list->GetSelections(items);
    m_list->Begin();
    for(auto item : items) {
        m_list->Delete(item);
    }
    m_list->Commit();
}

void DAPWatchesView::OnDeleteAll(wxCommandEvent& event)
{
    m_list->Begin();
    m_list->DeleteChildren(m_list->GetRootItem());
    m_list->Commit();
}

void DAPWatchesView::UpdateChildren(int varId, dap::VariablesResponse* response)
{
    if(!m_list) {
        return;
    }
    m_list->UpdateChildren(varId, response);
}

void DAPWatchesView::OnDeleteAllUI(wxUpdateUIEvent& event)
{
    wxTreeItemIdValue cookie;
    auto item = m_list->GetFirstChild(m_list->GetRootItem(), cookie);
    event.Enable(item.IsOk());
}

void DAPWatchesView::OnDeleteWatchUI(wxUpdateUIEvent& event)
{
    wxArrayTreeItemIds items;
    m_list->GetSelections(items);
    event.Enable(!items.empty());
}
