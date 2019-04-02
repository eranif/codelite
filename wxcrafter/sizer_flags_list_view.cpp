#include "sizer_flags_list_view.h"
#include "notebook_page_wrapper.h"
#include "wxc_project_metadata.h"
#include "wxc_widget.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "wxguicraft_main_view.h"
#include <algorithm>
#include <event_notifier.h>

SizerFlagsListView::SizerFlagsListView()
    : m_wxcWidget(NULL)
    , m_flags(NULL)
{
}

SizerFlagsListView::~SizerFlagsListView() {}

void SizerFlagsListView::Changed(wxPropertyGrid* pg, wxPropertyGridEvent& e)
{
    CHECK_POINTER(m_flags);
    CHECK_POINTER(m_wxcWidget);

    // Update the flags

    // Update wxGB* flags
    wxPGProperty* prop = e.GetProperty();
    wxString label = prop->GetLabel();
    wxString value = prop->GetValueAsString();

    if(label == _("Flags")) {
        bool showWxAll;
        if(pg->GetSelection()->GetLabel() == "wxALL") {
            showWxAll = pg->GetSelection()->GetValue().GetBool();
            if(showWxAll) {
                // No need to check if these are already present; duplicates will do no harm
                value << ",wxLEFT,wxRIGHT,wxTOP,wxBOTTOM";
            } else {
                // No need to worry about the commas; we use wxTOKEN_STRTOK below
                value.Replace("wxLEFT", "");
                value.Replace("wxRIGHT", "");
                value.Replace("wxTOP", "");
                value.Replace("wxBOTTOM", "");
            }

            wxArrayString arr = wxCrafter::Split(value, ",", wxTOKEN_STRTOK);
            m_wxcWidget->ClearSizerFlags();
            for(size_t i = 0; i < arr.GetCount(); ++i) {
                m_wxcWidget->EnableSizerFlag(arr.Item(i), true);
            }
            m_wxcWidget->EnableSizerFlag("wxALL", showWxAll);
        } else {
            showWxAll = value.Contains("wxLEFT") && value.Contains("wxRIGHT") && value.Contains("wxTOP") &&
                        value.Contains("wxBOTTOM");
            wxString flag_modified = pg->GetSelection()->GetLabel();
            bool flag_added = pg->GetSelection()->GetValue().GetBool();
            m_wxcWidget->EnableSizerFlag(flag_modified, flag_added);
            m_wxcWidget->EnableSizerFlag("wxALL", showWxAll);
        }
        Construct(pg, m_wxcWidget);
    }

    if(label == _("Proportion")) {
        m_wxcWidget->SizerItem().SetProportion(prop->GetValue().GetInteger());

    } else if(label == _("Border")) {
        m_wxcWidget->SizerItem().SetBorder(prop->GetValue().GetInteger());

    } else if(label == _("wxGBPosition")) {
        m_wxcWidget->SetGbPos(prop->GetValueAsString());

    } else if(label == _("wxGBSpan")) {
        m_wxcWidget->SetGbSpan(prop->GetValueAsString());
    }

    // Notify about modifications
    wxCommandEvent evt(wxEVT_PROPERTIES_MODIFIED);
    EventNotifier::Get()->AddPendingEvent(evt);
}

void SizerFlagsListView::Construct(wxPropertyGrid* pg, wxcWidget* wb)
{
    pg->Clear();
    m_flags = NULL;
    m_wxcWidget = wb;
    CHECK_POINTER(m_wxcWidget);

    if(m_wxcWidget->IsTopWindow() || !m_wxcWidget->IsSizerItem()) { return; }

    // Popuplate the sizer flags
    pg->Append(new wxIntProperty(_("Proportion"), wxPG_LABEL, m_wxcWidget->SizerItem().GetProportion()));
    pg->Append(new wxIntProperty(_("Border"), wxPG_LABEL, m_wxcWidget->SizerItem().GetBorder()));

    wxPGProperty* gbCategory(NULL);
    if(m_wxcWidget->IsGridBagSizerItem()) {
        pg->Append(new wxPropertyCategory(_("GridBagSizer")));
        pg->Append(new wxStringProperty(_("wxGBPosition"), wxPG_LABEL, m_wxcWidget->GetGbPos()));
        pg->Append(new wxStringProperty(_("wxGBSpan"), wxPG_LABEL, m_wxcWidget->GetGbSpan()));
    }

    pg->Append(new wxPropertyCategory(_("Sizer Flags")));

    wxcWidget::MapStyles_t flags = m_wxcWidget->GetSizerFlags();
    wxcWidget::MapStyles_t::ConstIterator iter = flags.Begin();

    wxArrayString styles;
    wxArrayInt stylesInt;
    size_t stylesValue = 0;

    // The control specific flags will show up first
    wxPGChoices choices;
    for(; iter != flags.End(); ++iter) {

        //// Rather than using the real bitvalue of the sizer flag, create our own fake one
        //// This is to avoid clashes between e.g. the composite wxALIGN_CENTER and its constituents
        /// wxALIGN_CENTER_VERTICAL and HORIZONTAL
        size_t flag_fake_value;
        if(iter->second.style_name == "wxALIGN_TOP") {
            // See the explanation inside enum SIZER_FLAG_VALUE
            flag_fake_value = wxcWidget::SizerFakeValue(crafterALIGN_TOP);

        } else if(iter->second.style_name == "wxRESERVE_SPACE_EVEN_IF_HIDDEN") {
            flag_fake_value = wxcWidget::SizerFakeValue(iter->second.style_bit);

        } else {
            flag_fake_value = wxcWidget::SizerFakeValue(iter->second.style_bit);
        }
        choices.Add(iter->second.style_name, flag_fake_value);

        // stylesInt.Add(flag_fake_value);         // Add the value
        if(iter->second.is_set) { stylesValue |= flag_fake_value; }
    }

    m_flags = (wxFlagsProperty*)pg->Append(new wxFlagsProperty(_("Flags"), wxPG_LABEL, choices, stylesValue));
#ifndef __WXMSW__
    pg->SetPropertyAttribute(m_flags, wxPG_BOOL_USE_CHECKBOX, true);
#endif
    // Set some tooltips to explain (at least to users who read tooltips) why some properties are disabled
    pg->SetPropertyHelpString("Flags.wxALIGN_BOTTOM", "This will be disabled in vertical boxsizers");
    pg->SetPropertyHelpString("Flags.wxALIGN_RIGHT", "This will be disabled in horizontal boxsizers");
    pg->SetPropertyHelpString("Flags.wxALIGN_CENTER_VERTICAL",
                              "This will be disabled in vertical boxsizers, and in horizontal boxsizers with wxEXPAND");
    pg->SetPropertyHelpString("Flags.wxALIGN_CENTER_HORIZONTAL",
                              "This will be disabled in horizontal boxsizers, and in vertical boxsizers with wxEXPAND");

    pg->Expand(m_flags);
    if(gbCategory) { pg->Collapse(gbCategory); }
    pg->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);
}

void SizerFlagsListView::DoUpdateUI(wxPropertyGrid* pg, wxUpdateUIEvent& event)
{
    CHECK_POINTER(m_wxcWidget);
    wxcWidget* parent = m_wxcWidget->GetParent();
    CHECK_POINTER(parent);
    if(!parent->GetWxClassName().Contains("BoxSizer")) { // we care only about wxBoxSizer and wxStaticBoxSizer
        return;
    }

    bool hasEXPAND = pg->GetPropertyValueAsString("Flags").Contains("wxEXPAND");
    bool hasACHoriz = pg->GetPropertyValueAsString("Flags").Contains("wxALIGN_CENTER_HORIZONTAL");
    bool hasACVert = pg->GetPropertyValueAsString("Flags").Contains("wxALIGN_CENTER_VERTICAL");

    if(parent->PropertyString(PROP_ORIENTATION) == "wxVERTICAL") {
        pg->EnableProperty("Flags.wxALIGN_BOTTOM", false);          // Can't have wxALIGN_BOTTOM in a vertical boxsizer
        pg->EnableProperty("Flags.wxALIGN_CENTER_VERTICAL", false); // Can't have wxALIGN_CENTER_VERTICAL in a vertical
                                                                    // boxsizer (unless it's part of wxALIGN_CENTER,
                                                                    // which here is a separate property)
        pg->EnableProperty("Flags.wxALIGN_CENTER_HORIZONTAL",
                           !hasEXPAND); // Can't have wxALIGN_CENTER_HORIZONTAL plus wxEXPAND in a vertical boxsizer
        pg->EnableProperty("Flags.wxEXPAND",
                           !hasACHoriz); // Still can't have wxALIGN_CENTER_HORIZONTAL plus wxEXPAND in a vertical boxsizer
        // Similarly for the relevant toolbar tools
        event.Enable(!(event.GetId() == ID_TOOL_ALIGN_VCENTER || event.GetId() == ID_TOOL_ALIGN_BOTTOM
                        || (event.GetId() == ID_TOOL_ALIGN_HCENTER) && hasEXPAND
                      )
                    );
        
    } else {
        pg->EnableProperty("Flags.wxALIGN_RIGHT", false); // Can't have wxALIGN_RIGHT in a horizontal boxsizer
        pg->EnableProperty("Flags.wxALIGN_CENTER_HORIZONTAL", false); // Can't have wxALIGN_CENTER_HORIZONTAL in a
                                                                      // horizontal boxsizer (unless it's part of
                                                                      // wxALIGN_CENTER, which here is a separate
                                                                      // property)
        pg->EnableProperty("Flags.wxALIGN_CENTER_VERTICAL",
                           !hasEXPAND); // Can't have wxALIGN_CENTER_VERTICAL plus wxEXPAND in a horizontal boxsizer
        pg->EnableProperty("Flags.wxEXPAND",
                           !hasACVert); // Can't have wxALIGN_CENTER_VERTICAL plus wxEXPAND in a horizontal boxsizer

        event.Enable(!(event.GetId() == ID_TOOL_ALIGN_HCENTER || event.GetId() == ID_TOOL_ALIGN_RIGHT
                        || (event.GetId() == ID_TOOL_ALIGN_VCENTER) && hasEXPAND
                      )
                    );
    }
}
