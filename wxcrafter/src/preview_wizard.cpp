#include "preview_wizard.h"
#include "DirectoryChanger.h"
#include "wxgui_helpers.h"
#include "wxguicraft_main_view.h"
#include <wx/app.h>
#include <wx/sstream.h>
#include <wx/xml/xml.h>

BEGIN_EVENT_TABLE(PreviewWizard, wxWizard)
EVT_CLOSE(PreviewWizard::OnClose)
EVT_WIZARD_CANCEL(wxID_ANY, PreviewWizard::OnWizard)
EVT_WIZARD_FINISHED(wxID_ANY, PreviewWizard::OnWizard)
END_EVENT_TABLE()

PreviewWizard::PreviewWizard(wxWindow* parent, const WizardWrapper& dw)
{
    wxString text;
    dw.ToXRC(text, wxcWidget::XRC_PREVIEW);
    wxXmlDocument doc;

    wxStringInputStream str(text);
    doc.Load(str);

    wxString xrcFilePath = wxFileName(wxCrafter::GetUserDataDir(), wxT("mywizard.xrc")).GetFullPath();
    doc.Save(xrcFilePath);

    DirectoryChanger dc(wxcProjectMetadata::Get().GetProjectPath());

    wxXmlResource::Get()->Load(xrcFilePath);
    wxXmlResource::Get()->LoadObject(this, parent, dw.GetName(), wxT("wxWizard"));
    wxXmlResource::Get()->Unload(xrcFilePath);

    Center();
    EventNotifier::Get()->Connect(wxEVT_CLOSE_PREVIEW, wxCommandEventHandler(PreviewWizard::OnClosePreview), NULL,
                                  this);
}

PreviewWizard::~PreviewWizard()
{
    EventNotifier::Get()->Disconnect(wxEVT_CLOSE_PREVIEW, wxCommandEventHandler(PreviewWizard::OnClosePreview), NULL,
                                     this);

    wxCommandEvent event(wxEVT_PREVIEW_CLOSED);
    EventNotifier::Get()->AddPendingEvent(event);
}

void PreviewWizard::OnClosePreview(wxCommandEvent& e)
{
    e.Skip();
    // Destroy();
}

void PreviewWizard::OnClose(wxCloseEvent& e)
{
    e.Skip();
    // Destroy();
}

void PreviewWizard::Run()
{
    wxWizardPageSimple* pageOne = XRCCTRL((*this), "WIZARD_PAGE_ONE", wxWizardPageSimple);
    if(pageOne) {
        GetPageAreaSizer()->Add(pageOne);
        RunWizard(pageOne);
    }
}

void PreviewWizard::OnWizard(wxWizardEvent& e)
{
    e.Skip();
    // Destroy();
}
