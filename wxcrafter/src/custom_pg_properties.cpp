#include "custom_pg_properties.h"
#include "color_property.h"
#include "wxguicraft_main_view.h"
#include <event_notifier.h>

bool wxPG_Colour::OnEvent(wxPropertyGrid* propgrid, wxWindow* primary, wxEvent& event)
{
    if(propgrid->IsMainButtonEvent(event)) {
        SetValueToUnspecified();
        ColorProperty* pb = reinterpret_cast<ColorProperty*>(GetClientData());
        if(pb) { pb->SetValue("<Default>"); }

        // Notify about modifications
        wxCommandEvent evt(wxEVT_PROPERTIES_MODIFIED);
        if(pb->GetLabel() == PROP_NAME) { evt.SetString(GetValueAsString()); }
        EventNotifier::Get()->AddPendingEvent(evt);
        return true;

    } else {
        return wxSystemColourProperty::OnEvent(propgrid, primary, event);
    }
}

// -----------------------------------------------------------------------
// wxcPGChoiceAndButtonEditor
// -----------------------------------------------------------------------

wxPGWindowList wxcPGChoiceAndButtonEditor::CreateControls(wxPropertyGrid* propGrid, wxPGProperty* property,
                                                          const wxPoint& pos, const wxSize& sz) const
{
    // Use one two units smaller to match size of the combo's dropbutton.
    // (normally a bigger button is used because it looks better)
    int bt_wid = sz.y;
    bt_wid -= 2;
    wxSize bt_sz(bt_wid, bt_wid);

    // Position of button.
    wxPoint bt_pos(pos.x + sz.x - bt_sz.x, pos.y);
#ifdef __WXMAC__
    bt_pos.y -= 1;
#else
    bt_pos.y += 1;
#endif

    wxWindow* bt = propGrid->GenerateEditorButton(bt_pos, bt_sz);
    bt->SetToolTip(_("Clear"));
    wxButton* button = dynamic_cast<wxButton*>(bt);
    if(button) { button->SetLabel("X"); }

    // Size of choice.
    wxSize ch_sz(sz.x - bt->GetSize().x, sz.y);
#if 0
#ifdef __WXMAC__
    ch_sz.x -= wxPG_TEXTCTRL_AND_BUTTON_SPACING;
#endif
#endif

    wxWindow* ch = wxPGEditor_Choice->CreateControls(propGrid, property, pos, ch_sz).m_primary;

#ifdef __WXMSW__
    bt->Show();
#endif

    return wxPGWindowList(ch, bt);
}

wxcPGChoiceAndButtonEditor::~wxcPGChoiceAndButtonEditor() {}

wxString wxcPGChoiceAndButtonEditor::GetName() const { return GetClassInfo()->GetClassName(); }
