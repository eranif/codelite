#include "SelectDropTargetDlg.h"

#include "clWorkspaceView.h"
#include "codelite_events.h"
#include "globals.h"
#include "imanager.h"

#include <map>

SelectDropTargetDlg::SelectDropTargetDlg(wxWindow* parent, const wxArrayString& folders)
    : SelectDropTargetBaseDlg(parent)
    , m_folders(folders)
{
    Initialize();
}

void SelectDropTargetDlg::Initialize()
{
    m_views = clGetManager()->GetWorkspaceView()->GetAllPages();
    for (const auto& p : m_views) {
        wxVector<wxVariant> cols;
        cols.push_back(p.first);
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)p.second);
    }
}

void SelectDropTargetDlg::OnOKUI(wxUpdateUIEvent& event) { event.Enable(m_dvListCtrl->HasSelection()); }

void SelectDropTargetDlg::OnOK(wxCommandEvent& event)
{
    event.Skip();
    ActivateSelection();
    EndModal(wxID_OK);
}

void SelectDropTargetDlg::OnSelectionActivated(wxDataViewEvent& event)
{
    ActivateSelection();
    EndModal(wxID_OK);
}

void SelectDropTargetDlg::ActivateSelection()
{
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    if (item.IsOk()) {
        wxWindow* page = reinterpret_cast<wxWindow*>(m_dvListCtrl->GetItemData(item));
        if (page) {
            clCommandEvent event(wxEVT_DND_FOLDER_DROPPED);
            event.SetStrings(m_folders);
            page->GetEventHandler()->AddPendingEvent(event);
        }
    }
}
