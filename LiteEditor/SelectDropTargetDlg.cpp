#include "SelectDropTargetDlg.h"
#include "imanager.h"
#include "globals.h"
#include "clWorkspaceView.h"
#include <map>
#include "codelite_events.h"

SelectDropTargetDlg::SelectDropTargetDlg(wxWindow* parent, const wxArrayString& folders)
    : SelectDropTargetBaseDlg(parent)
    , m_folders(folders)
{
    Initialize();
}

SelectDropTargetDlg::~SelectDropTargetDlg() {}

void SelectDropTargetDlg::OnButtonSelected(wxCommandEvent& event) 
{
    // Propogate the event to the correct handler
    wxCommandLinkButton* btn = dynamic_cast<wxCommandLinkButton*>(event.GetEventObject());
    if(btn) {
        wxWindow* page = reinterpret_cast<wxWindow*>(btn->GetClientData());
        if(page) {
            clCommandEvent event(wxEVT_DND_FOLDER_DROPPED);
            event.SetStrings(m_folders);
            page->GetEventHandler()->AddPendingEvent(event);
        }
    }
    EndModal(wxID_OK);
}

void SelectDropTargetDlg::Initialize()
{
    m_views = clGetManager()->GetWorkspaceView()->GetAllPages();
    std::for_each(m_views.begin(), m_views.end(), [&](const std::pair<wxString, wxWindow*>& p) {
        wxString label, desc;
        label << p.first;
        desc << _("Open the folder in the '") << label << _("' view");

        wxCommandLinkButton* btn = new wxCommandLinkButton(this, wxID_ANY, label, desc);
        GetSizer()->Add(btn, 0, wxALL | wxEXPAND, 5);
        btn->Bind(wxEVT_BUTTON, &SelectDropTargetDlg::OnButtonSelected, this);
        btn->SetClientData(p.second); // Associate the window with the button
    });
    GetSizer()->Layout();
    GetSizer()->Fit(this);
}
