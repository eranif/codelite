#include "TweaksSettingsDlg.h"
#include "tweaks_settings.h"
#include "workspace.h"
#include <wx/choicdlg.h>
#include "windowattrmanager.h"
#include "ProjectColoursDlg.h"
#include "macros.h"
#include <wx/msgdlg.h>

TweaksSettingsDlg::TweaksSettingsDlg(wxWindow* parent)
    : TweaksSettingsDlgBase(parent)
{
    DoPopulateList();
    WindowAttrManager::Load(this, "TweaksSettingsDlg", NULL);
}

TweaksSettingsDlg::~TweaksSettingsDlg()
{
    WindowAttrManager::Save(this, "TweaksSettingsDlg", NULL);
}

void TweaksSettingsDlg::OnAddProject(wxCommandEvent& event)
{
    ProjectColoursDlg dlg(this);
    if ( dlg.ShowModal() == wxID_OK ) {
        ProjectTweaks pt = dlg.GetSelection();
        if ( !IsExists( pt.GetProjectName() ) ) {
            wxVector<wxVariant> cols;
            cols.push_back( pt.GetProjectName() );
            m_dvListCtrlProjects->AppendItem(cols);
        }
    }
}

void TweaksSettingsDlg::OnWorkspaceOpenUI(wxUpdateUIEvent& event)
{
    event.Enable( WorkspaceST::Get()->IsOpen() );
}

void TweaksSettingsDlg::OnEdit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxDataViewItem sel = m_dvListCtrlProjects->GetSelection();
    CHECK_ITEM_RET(sel);

    wxVariant value;
    m_dvListCtrlProjects->GetValue(value, m_dvListCtrlProjects->ItemToRow(sel), 0);
    ProjectColoursDlg dlg(this, value.GetString());
    if ( dlg.ShowModal() == wxID_OK ) {

    }
}

void TweaksSettingsDlg::OnEditUI(wxUpdateUIEvent& event)
{
    event.Enable( WorkspaceST::Get()->IsOpen() && m_dvListCtrlProjects->GetSelectedItemsCount() );
}

bool TweaksSettingsDlg::IsExists(const wxString& projectName) const
{
    int count = m_dvListCtrlProjects->GetItemCount();
    for( int i = 0; i<count; ++i ) {
        wxVariant value;
        m_dvListCtrlProjects->GetValue(value, i, 0);
        if ( projectName == value.GetString() )
            return true;
    }
    return false;
}

void TweaksSettingsDlg::OnItemActivated(wxDataViewEvent& event)
{
    wxCommandEvent dummy;
    OnEdit(dummy);
}

void TweaksSettingsDlg::OnDelete(wxCommandEvent& event)
{
    wxDataViewItem sel = m_dvListCtrlProjects->GetSelection();
    CHECK_ITEM_RET(sel);

    wxVariant value;
    m_dvListCtrlProjects->GetValue(value, m_dvListCtrlProjects->ItemToRow(sel), 0);

    int answer = ::wxMessageBox(wxString() << _("Are you sure you want to delete tweaks for the project '") << value.GetString() << "' ? ", _("Confirm"), wxICON_QUESTION|wxYES_NO|wxCANCEL|wxCENTER, this);
    if ( answer == wxYES ) {
        TweaksSettings settings;
        settings.Load();
        settings.DeleteProject( value.GetString() );
        settings.Save();
        DoPopulateList();
    }
}

void TweaksSettingsDlg::DoPopulateList()
{
    TweaksSettings settings;
    settings.Load();

    m_dvListCtrlProjects->DeleteAllItems();
    ProjectTweaks::Map_t::const_iterator iter = settings.GetProjects().begin();
    for( ; iter != settings.GetProjects().end(); ++iter ) {
        wxVector<wxVariant> cols;
        cols.push_back( iter->first );
        m_dvListCtrlProjects->AppendItem(cols);
    }
    m_colourPickerGlobalBG->SetColour( settings.GetGlobalBgColour() );
    m_colourPickerGlobalFG->SetColour( settings.GetGlobalFgColour() );
    m_checkBox78->SetValue( settings.IsEnableTweaks() );
}

void TweaksSettingsDlg::OnGlobalBgColourChanged(wxColourPickerEvent& event)
{
    TweaksSettings settings;
    settings.Load();
    settings.SetGlobalBgColour( event.GetColour() );
    settings.Save();
}

void TweaksSettingsDlg::OnGlobalFgColourChanged(wxColourPickerEvent& event)
{
    TweaksSettings settings;
    settings.Load();
    settings.SetGlobalFgColour( event.GetColour() );
    settings.Save();
}

void TweaksSettingsDlg::OnEnableTweaks(wxCommandEvent& event)
{
    TweaksSettings settings;
    settings.Load();
    settings.SetEnableTweaks( event.IsChecked() );
    settings.Save();
}

void TweaksSettingsDlg::OnEnableTweaksUI(wxUpdateUIEvent& event)
{
    event.Enable( m_checkBox78->IsChecked() && WorkspaceST::Get()->IsOpen() );
}

void TweaksSettingsDlg::OnEnableTweaksCheckboxUI(wxUpdateUIEvent& event)
{
    event.Enable( WorkspaceST::Get()->IsOpen() );
}
