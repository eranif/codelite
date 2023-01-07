#include "preview_frame.h"
#include "DirectoryChanger.h"
#include "wxc_aui_manager.h"
#include "wxgui_helpers.h"
#include "wxguicraft_main_view.h"
#include <wx/app.h>
#include <wx/sizer.h>
#include <wx/sstream.h>
#include <wx/xml/xml.h>

BEGIN_EVENT_TABLE(PreviewFrame, wxFrame)
END_EVENT_TABLE()

PreviewFrame::PreviewFrame(wxWindow* parent, const TopLevelWinWrapper& fw)
{
    wxSize sz = wxCrafter::DecodeSize(fw.SizeAsString());
    wxString text;

    fw.ToXRC(text, wxcWidget::XRC_PREVIEW);
    wxXmlDocument doc;

    wxStringInputStream str(text);
    doc.Load(str);

    wxString xrcFilePath = wxFileName(wxCrafter::GetUserDataDir(), wxT("myframe.xrc")).GetFullPath();
    doc.Save(xrcFilePath);
    DirectoryChanger dc(wxcProjectMetadata::Get().GetProjectPath());

    wxXmlResource::Get()->Load(xrcFilePath);
    wxXmlResource::Get()->LoadFrame(this, parent, fw.GetName());
    wxXmlResource::Get()->Unload(xrcFilePath);

    Center();
    SetSizeHints(sz);

    if(wxcAuiManager::Get().Find(this)) {
        wxcAuiManager::Get().Find(this)->Update();
    } else {
        Layout();
    }

    EventNotifier::Get()->Connect(wxEVT_CLOSE_PREVIEW, wxCommandEventHandler(PreviewFrame::OnClosePreview), NULL, this);
}

PreviewFrame::~PreviewFrame()
{
    wxcAuiManager::Get().UnInit(this);
    EventNotifier::Get()->Disconnect(wxEVT_CLOSE_PREVIEW, wxCommandEventHandler(PreviewFrame::OnClosePreview), NULL,
                                     this);
    wxCommandEvent event(wxEVT_PREVIEW_CLOSED);
    EventNotifier::Get()->AddPendingEvent(event);
}

void PreviewFrame::OnClosePreview(wxCommandEvent& e)
{
    e.Skip();
    Close();
}
