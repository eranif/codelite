#include "DAPTooltip.hpp"

#include "DAPVariableListCtrl.hpp"
#include "clResizableTooltip.h" // wxEVT_TOOLTIP_DESTROY
#include "clSystemSettings.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "globals.h"
#include "macros.h"

#include <wx/dcclient.h>
#include <wx/sizer.h>

DAPTooltip::DAPTooltip(dap::Client* client, const wxString& expression, const wxString& result, const wxString& type,
                       int variableReference)
    : wxPopupWindow(EventNotifier::Get()->TopFrame())
    , m_client(client)
{
    // set a reasonable size for the tooltip
    wxClientDC dc(this);
    dc.SetFont(DrawingUtils::GetDefaultGuiFont());
    wxSize sz = dc.GetTextExtent("Tp");
    sz.SetWidth(sz.GetWidth() * 80);   // 50 chars width
    sz.SetHeight(sz.GetHeight() * 20); // 10 lines

    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_list = new DAPVariableListCtrl(this, client, dap::EvaluateContext::HOVER, wxID_ANY, wxDefaultPosition, sz);
    GetSizer()->Add(m_list, 1, wxEXPAND);
    m_list->AddWatch(expression, result, type, variableReference);
    m_list->Bind(wxEVT_KEY_DOWN, &DAPTooltip::OnKeyDown, this);
    GetSizer()->Fit(this);
}

DAPTooltip::~DAPTooltip() {}

void DAPTooltip::UpdateChildren(int varId, dap::VariablesResponse* response)
{
    m_list->UpdateChildren(varId, response);
}

void DAPTooltip::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    if(event.GetKeyCode() == WXK_ESCAPE) {
        //  Cancel this tip
        clCommandEvent destroyEvent(wxEVT_TOOLTIP_DESTROY);
        EventNotifier::Get()->AddPendingEvent(destroyEvent);
    }
}
