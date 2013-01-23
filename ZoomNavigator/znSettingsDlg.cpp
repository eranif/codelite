#include "znSettingsDlg.h"
#include "windowattrmanager.h"
#include "zn_config_item.h"
#include "cl_config.h"
#include "event_notifier.h"

const wxEventType wxEVT_ZN_SETTINGS_UPDATED = ::wxNewEventType();

znSettingsDlg::znSettingsDlg(wxWindow* parent)
    : znSettingsDlgBase(parent)
{
    znConfigItem data;
    clConfig conf("zoom-navigator.conf");
    if ( conf.ReadItem(&data) ) {
        m_checkBoxEnableZN->SetValue(data.IsEnabled());
        m_colourPickerHighlightColour->SetColour( wxColour( data.GetHighlightColour() ));
    }

    WindowAttrManager::Load(this, "znSettingsDlg", NULL);
}

znSettingsDlg::~znSettingsDlg()
{
    WindowAttrManager::Save(this, "znSettingsDlg", NULL);
}

void znSettingsDlg::OnOK(wxCommandEvent& event)
{
    znConfigItem data;
    clConfig conf("zoom-navigator.conf");
    data.SetEnabled( m_checkBoxEnableZN->IsChecked() );
    data.SetHighlightColour( m_colourPickerHighlightColour->GetColour().GetAsString(wxC2S_HTML_SYNTAX) );
    conf.WriteItem( &data );
    
    // notify thet the settings were changed
    wxCommandEvent evt(wxEVT_ZN_SETTINGS_UPDATED);
    EventNotifier::Get()->AddPendingEvent( evt );
    
    EndModal(wxID_OK);
}
