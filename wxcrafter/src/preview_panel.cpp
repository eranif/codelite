#include "preview_panel.h"
#include "DirectoryChanger.h"
#include "wxgui_helpers.h"
#include "wxguicraft_main_view.h"
#include <event_notifier.h>
#include <wx/app.h>
#include <wx/sizer.h>
#include <wx/sstream.h>
#include <wx/xml/xml.h>

BEGIN_EVENT_TABLE(PreviewPanel, wxFrame)
END_EVENT_TABLE()

PreviewPanel::PreviewPanel(wxWindow* parent, const TopLevelWinWrapper& fw)
    : wxFrame(parent, wxID_ANY, wxT("Preview"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP)
{
    wxString text;
    fw.ToXRC(text, wxcWidget::XRC_PREVIEW);
    wxXmlDocument doc;

    wxStringInputStream str(text);
    doc.Load(str);

    wxString xrcFilePath = wxFileName(wxCrafter::GetUserDataDir(), wxT("mypanel.xrc")).GetFullPath();

    doc.Save(xrcFilePath);
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);

    DirectoryChanger dc(wxcProjectMetadata::Get().GetProjectPath());

    wxXmlResource::Get()->Load(xrcFilePath);
    wxString previewPanelName = fw.GetName();
    wxPanel* panel = wxXmlResource::Get()->LoadPanel(this, previewPanelName);
    wxXmlResource::Get()->Unload(xrcFilePath);
    sz->Add(panel, 1, wxEXPAND);

    Center();
    sz->Layout();
    GetSizer()->Fit(this);
    EventNotifier::Get()->Connect(wxEVT_CLOSE_PREVIEW, wxCommandEventHandler(PreviewPanel::OnClosePreviewPreviewPanel),
                                  NULL, this);
}

PreviewPanel::~PreviewPanel()
{
    EventNotifier::Get()->Disconnect(wxEVT_CLOSE_PREVIEW,
                                     wxCommandEventHandler(PreviewPanel::OnClosePreviewPreviewPanel), NULL, this);
    wxCommandEvent event(wxEVT_PREVIEW_CLOSED);
    EventNotifier::Get()->AddPendingEvent(event);
}

void PreviewPanel::OnClosePreviewPreviewPanel(wxCommandEvent& e)
{
    e.Skip();
    Close();
}
