#include "ExternalToolsManager.h"
#include "ExternalToolsProcessManager.h"
#include "bitmap_loader.h"
#include "globals.h"
#include "imanager.h"
#include <algorithm>

ExternalToolsManager::ExternalToolsManager(wxWindow* parent)
    : ExternalToolsManagerBase(parent)
{
    DoPopulateTable();
}

ExternalToolsManager::~ExternalToolsManager() {}

void ExternalToolsManager::OnKill(wxCommandEvent& event)
{
    wxDataViewItemArray arr;
    m_dvListCtrlTasks->GetSelections(arr);
    for(size_t i = 0; i < arr.size(); ++i) {
        wxDataViewItem item = arr.Item(i);
        ExternalToolItemData* cd = (ExternalToolItemData*)m_dvListCtrlTasks->GetItemData(item);
        ToolsTaskManager::Instance()->Stop(cd->m_pid);
    }
    DoPopulateTable();
}

void ExternalToolsManager::OnKillAll(wxCommandEvent& event)
{
    ToolsTaskManager::Instance()->StopAll();
    DoPopulateTable();
}

void ExternalToolsManager::OnKillAllUI(wxUpdateUIEvent& event) { event.Enable(m_dvListCtrlTasks->GetItemCount()); }
void ExternalToolsManager::OnKillUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlTasks->GetSelectedItemsCount());
}

void ExternalToolsManager::DoPopulateTable()
{
    DoClear();
    BitmapLoader* b = clGetManager()->GetStdIcons();
    const ExternalToolItemData::Map_t& tools = ToolsTaskManager::Instance()->GetTools();
    std::for_each(tools.begin(), tools.end(), [&](const std::pair<int, ExternalToolItemData>& p) {
        wxVector<wxVariant> cols;
        cols.push_back(::MakeIconText(wxString() << p.first, b->LoadBitmap("cog")));
        cols.push_back(p.second.m_command);
        m_dvListCtrlTasks->AppendItem(cols, (wxUIntPtr)p.second.Clone());
    });
}

void ExternalToolsManager::DoClear()
{
    for(size_t i = 0; i < m_dvListCtrlTasks->GetItemCount(); ++i) {
        ExternalToolItemData* cd =
            (ExternalToolItemData*)m_dvListCtrlTasks->GetItemData(m_dvListCtrlTasks->RowToItem(i));
        delete cd;
    }
    m_dvListCtrlTasks->DeleteAllItems();
}
void ExternalToolsManager::OnRefresh(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoPopulateTable();
}
