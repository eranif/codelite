#include "gitSettingsDlg.h"
#include "windowattrmanager.h"
#include "cl_config.h"
#include "gitentry.h"
#include "event_notifier.h"

GitSettingsDlg::GitSettingsDlg(wxWindow* parent)
    :GitSettingsDlgBase(parent)
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    m_pathGIT->SetPath(data.GetGITExecutablePath());
    m_pathGITK->SetPath(data.GetGITKExecutablePath());
    m_checkBoxLog->SetValue( data.GetFlags() & GitEntry::Git_Verbose_Log );
    m_checkBoxTerminal->SetValue( data.GetFlags() & GitEntry::Git_Show_Terminal );
    m_checkBoxTrackTree->SetValue( data.GetFlags() & GitEntry::Git_Colour_Tree_View );
    
    WindowAttrManager::Load(this, wxT("GitSettingsDlg"), NULL);
}

GitSettingsDlg::~GitSettingsDlg()
{
    WindowAttrManager::Save(this, wxT("GitSettingsDlg"), NULL);
}

void GitSettingsDlg::OnOK(wxCommandEvent& event)
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    data.SetGITExecutablePath( m_pathGIT->GetPath() );
    data.SetGITKExecutablePath( m_pathGITK->GetPath() );
    
    size_t flags = 0;
    if ( m_checkBoxLog->IsChecked() )
        flags |= GitEntry::Git_Verbose_Log;
        
    if ( m_checkBoxTerminal->IsChecked() )
        flags |= GitEntry::Git_Show_Terminal;
        
    if ( m_checkBoxTrackTree->IsChecked()) 
        flags |= GitEntry::Git_Colour_Tree_View;
        
    data.SetFlags( flags );
    conf.WriteItem(&data);
    
    // Notify about configuration changed
    wxCommandEvent evt(wxEVT_GIT_CONFIG_CHANGED);
    EventNotifier::Get()->AddPendingEvent( evt );
    
    EndModal(wxID_OK);
}
