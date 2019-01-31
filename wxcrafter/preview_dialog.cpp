#include "preview_dialog.h"
#include "DirectoryChanger.h"
#include "wxc_aui_manager.h"
#include "wxgui_helpers.h"
#include "wxguicraft_main_view.h"
#include <wx/app.h>
#include <wx/sstream.h>
#include <wx/xml/xml.h>

BEGIN_EVENT_TABLE(PreviewDialog, wxDialog)
EVT_CLOSE(PreviewDialog::OnClose)
END_EVENT_TABLE()
PreviewDialog::PreviewDialog(wxWindow* parent, const DialogWrapper& dw)
{
    wxSize sz = wxCrafter::DecodeSize(dw.SizeAsString());

    wxString text;
    dw.ToXRC(text, wxcWidget::XRC_PREVIEW);
    wxXmlDocument doc;

    wxStringInputStream str(text);
    doc.Load(str);

    wxString xrcFilePath = wxFileName(wxCrafter::GetUserDataDir(), wxT("mydlg.xrc")).GetFullPath();
    doc.Save(xrcFilePath);
    DirectoryChanger dc(wxcProjectMetadata::Get().GetProjectPath());

    wxXmlResource::Get()->Load(xrcFilePath);
    wxXmlResource::Get()->LoadDialog(this, parent, dw.GetName());
    wxXmlResource::Get()->Unload(xrcFilePath);

    {
        //        wxLogNull nl;
        //        wxRemoveFile(xrcFile.GetFullPath());
    }

    Center();
    SetSizeHints(sz);

    Layout();
    EventNotifier::Get()->Connect(wxEVT_CLOSE_PREVIEW, wxCommandEventHandler(PreviewDialog::OnClosePreview), NULL,
                                  this);
}

PreviewDialog::~PreviewDialog()
{
    wxcAuiManager::Get().UnInit(this);
    EventNotifier::Get()->Disconnect(wxEVT_CLOSE_PREVIEW, wxCommandEventHandler(PreviewDialog::OnClosePreview), NULL,
                                     this);

    wxCommandEvent event(wxEVT_PREVIEW_CLOSED);
    EventNotifier::Get()->AddPendingEvent(event);
}

void PreviewDialog::OnClosePreview(wxCommandEvent& e) { Destroy(); }

void PreviewDialog::OnClose(wxCloseEvent& e)
{
    e.Skip();
    Destroy();
}
