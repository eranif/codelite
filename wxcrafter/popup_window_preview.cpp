#include "popup_window_preview.h"
#include "DirectoryChanger.h"
#include "wxc_project_metadata.h"
#include "wxgui_helpers.h"
#include "wxguicraft_main_view.h"
#include <event_notifier.h>
#include <wx/sstream.h>
#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(PopupWindowPreview, wxFrame)
END_EVENT_TABLE()
PopupWindowPreview::PopupWindowPreview(wxWindow* parent, const PopupWindowWrapper& wrapper)
    : wxFrame(parent, wxID_ANY, wxT("Preview"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP)
{
    wxString text;
    wrapper.ToXRC(text, wxcWidget::XRC_PREVIEW);
    wxXmlDocument doc;

    wxStringInputStream str(text);
    doc.Load(str);

    wxString xrcFilePath = wxFileName(wxCrafter::GetUserDataDir(), wxT("mypanel.xrc")).GetFullPath();

    doc.Save(xrcFilePath);
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);

    DirectoryChanger dc(wxcProjectMetadata::Get().GetProjectPath());

    wxXmlResource::Get()->Load(xrcFilePath);
    wxPanel* panel = wxXmlResource::Get()->LoadPanel(this, wrapper.GetName());
    wxXmlResource::Get()->Unload(xrcFilePath);
    sz->Add(panel, 1, wxEXPAND);

    Center();
    sz->Layout();
    GetSizer()->Fit(this);
    EventNotifier::Get()->Connect(wxEVT_CLOSE_PREVIEW,
                                  wxCommandEventHandler(PopupWindowPreview::OnClosePreviewPreviewPanel), NULL, this);
}

PopupWindowPreview::~PopupWindowPreview()
{
    EventNotifier::Get()->Disconnect(wxEVT_CLOSE_PREVIEW,
                                     wxCommandEventHandler(PopupWindowPreview::OnClosePreviewPreviewPanel), NULL, this);
    wxCommandEvent event(wxEVT_PREVIEW_CLOSED);
    EventNotifier::Get()->AddPendingEvent(event);
}

void PopupWindowPreview::OnClosePreviewPreviewPanel(wxCommandEvent& e)
{
    e.Skip();
    Close();
}
