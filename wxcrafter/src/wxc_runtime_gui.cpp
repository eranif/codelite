// GUI implementation of the wxCrafter runtime shim.
// Linked into the wxcrafter executable and the wxCrafterPlugin shared library.

#include "wxc_runtime.h"

#include "event_notifier.h"
#include "main.h"
#include "wxgui_bitmaploader.h"
#include "wxguicraft_main_view.h"

#include <wx/frame.h>
#include <wx/imaglist.h>
#include <wx/utils.h>

namespace wxc_runtime
{

wxImageList* CreateAllocatorImageList() { return new wxImageList(16, 16, true); }

int AddImageToAllocator(wxImageList* list, const wxCrafter::ResourceLoader& loader, const wxString& bmpName)
{
    if (!list) {
        return -1;
    }
    return list->Add(loader.Bitmap(bmpName));
}

BitmapGenStatusScope::BitmapGenStatusScope()
{
    wxFrame* topFrame = EventNotifier::Get()->TopFrame();
    if (topFrame) {
        topFrame->SetStatusText("Generating bitmap code...");
    }
    wxBeginBusyCursor();
}

BitmapGenStatusScope::~BitmapGenStatusScope()
{
    if (wxIsBusy()) {
        wxEndBusyCursor();
    }
    wxFrame* topFrame = EventNotifier::Get()->TopFrame();
    if (topFrame) {
        topFrame->SetStatusText("Ready");
    }
}

void SetStatusMessage(const wxString& msg)
{
    wxFrame* topFrame = EventNotifier::Get()->TopFrame();
    if (!topFrame) {
        return;
    }
    if (MainFrame* frame = dynamic_cast<MainFrame*>(topFrame)) {
        frame->SetStatusMessage(msg);
    }
}

bool IsActiveWizardPage(const wxcWidget* page)
{
    if (!GUICraftMainPanel::m_MainPanel) {
        return false;
    }
    return GUICraftMainPanel::m_MainPanel->GetActiveWizardPage() == page;
}

void RequestDesignerRefresh()
{
    wxCommandEvent evt(wxEVT_REFRESH_DESIGNER);
    EventNotifier::Get()->AddPendingEvent(evt);
}

} // namespace wxc_runtime
