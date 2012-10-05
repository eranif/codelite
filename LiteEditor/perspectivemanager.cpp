#include "perspectivemanager.h"
#include "frame.h"
#include "editor_config.h"
#include <wx/stdpaths.h>
#include <wx/aui/framemanager.h>
#include "globals.h"

wxString DEBUG_LAYOUT  = wxT("debug.layout");
wxString NORMAL_LAYOUT = wxT("default.layout");

PerspectiveManager::PerspectiveManager()
    : m_active(wxT("Default"))
    , m_aui(NULL)
{
    wxString active = EditorConfigST::Get()->GetStringValue(wxT("ActivePerspective"));
    if(active.IsEmpty() == false) {
        m_active = active;
    }
    ClearIds();
}

PerspectiveManager::~PerspectiveManager()
{
    DisconnectEvents();
}

void PerspectiveManager::DeleteAllPerspectives()
{
    wxArrayString files;
    wxDir::GetAllFiles(wxStandardPaths::Get().GetUserDataDir() + wxT("/config/"), &files, wxT("*.layout"));

    wxLogNull noLog;
    for(size_t i=0; i<files.GetCount(); i++) {
        wxRemoveFile(files.Item(i));
    }
}

void PerspectiveManager::LoadPerspective(const wxString& name)
{
    wxString pname = name;
    if(pname.IsEmpty()) {
        pname = m_active;
    }

    wxString file = DoGetPathFromName(pname);
    wxString nameOnly;

    if(name.Find(wxT(".")) == wxNOT_FOUND) {
        nameOnly = name;
    } else {
        nameOnly = name.BeforeLast(wxT('.'));
    }

//	// check whether the perspective has been modified before switching
//	// in such case - update the active perspective and then
//	// load the new one
//	if(nameOnly.CmpNoCase(m_active) != 0) {
//		wxString activeContent;
//		wxString activeFileName = DoGetPathFromName(m_active);
//		wxString perspective    = clMainFrame::Get()->GetDockingManager().SavePerspective();
//
//		if(ReadFileWithConversion(activeFileName, activeContent) && activeContent != perspective) {
//			// Need to update the active perspective
//			WriteFileWithBackup(DoGetPathFromName(m_active), perspective, false);
//		}
//	}

    wxString content;
    if(ReadFileWithConversion(file, content)) {
        clMainFrame::Get()->GetDockingManager().LoadPerspective(content);
        m_active = nameOnly;
        EditorConfigST::Get()->SaveStringValue(wxT("ActivePerspective"), m_active);

        if(name == DEBUG_LAYOUT) {
            DoEnsureDebuggerPanesAreVisible();
        }

    } else {
        if(name == DEBUG_LAYOUT) {
            DoEnsureDebuggerPanesAreVisible();

            SavePerspective(name);
            m_active = nameOnly;
            EditorConfigST::Get()->SaveStringValue(wxT("ActivePerspective"), m_active);

        } else if (name == NORMAL_LAYOUT) {
            // Requested to load the Normal layout but we got no such layout
            // Make the current one the default layout
            SavePerspective(name);
            m_active = nameOnly;
            EditorConfigST::Get()->SaveStringValue(wxT("ActivePerspective"), m_active);
        }
    }

//#ifdef __WXMAC__
//	clMainFrame::Get()->GetDockingManager().Update();
//#endif

}

void PerspectiveManager::SavePerspective(const wxString& name, bool notify)
{
    wxString pname = name;
    if(pname.IsEmpty()) {
        pname = GetActive();
    }
    WriteFileWithBackup(DoGetPathFromName(pname),
                        clMainFrame::Get()->GetDockingManager().SavePerspective(),
                        false);

    if(pname.Find(wxT(".")) == wxNOT_FOUND) {
        m_active = pname;
    } else {
        m_active = pname.BeforeLast(wxT('.'));
    }
    EditorConfigST::Get()->SaveStringValue(wxT("ActivePerspective"), m_active);
    if(notify) {
        wxCommandEvent evt(wxEVT_REFRESH_PERSPECTIVE_MENU);
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evt);
    }
}

wxArrayString PerspectiveManager::GetAllPerspectives()
{
    wxArrayString files, perspectives;
    wxDir::GetAllFiles(wxStandardPaths::Get().GetUserDataDir() + wxT("/config/"), &files, wxT("*.layout"));

    for(size_t i=0; i<files.GetCount(); i++) {
        wxFileName fn(files.Item(i));
        wxString name = fn.GetName();
#if wxVERSION_NUMBER >= 2900
        name = name.Capitalize();
#else
        name.MakeLower();
        wxString start  = name.Mid(0, 1);
        start.MakeUpper();
        name[0] = start[0];
#endif
        perspectives.Add(name);
    }
    return perspectives;
}

void PerspectiveManager::ClearIds()
{
    m_menuIdToName.clear();
    m_nextId = FirstMenuId();
}

int PerspectiveManager::MenuIdFromName(const wxString& name)
{
    std::map<wxString, int>::iterator iter = m_menuIdToName.find(name);
    if(iter == m_menuIdToName.end()) {
        m_menuIdToName[name] = ++m_nextId;
        return m_menuIdToName[name];
    }
    return iter->second;
}

wxString PerspectiveManager::NameFromMenuId(int id)
{
    std::map<wxString, int>::iterator iter = m_menuIdToName.begin();
    for(; iter != m_menuIdToName.end(); iter++) {
        if(iter->second == id) {
            return iter->first;
        }
    }
    return wxT("");
}

void PerspectiveManager::LoadPerspectiveByMenuId(int id)
{
    wxString name = NameFromMenuId(id);
    if(name.IsEmpty())
        return;

    LoadPerspective(name);
}

void PerspectiveManager::Delete(const wxString& name)
{
    wxLogNull noLog;
    wxString path = DoGetPathFromName(name);
    wxRemoveFile(path);
}

void PerspectiveManager::Rename(const wxString& old, const wxString& new_name)
{
    wxString oldPath = DoGetPathFromName(old);
    wxString newPath = DoGetPathFromName(new_name);

    wxLogNull noLog;
    wxRename(oldPath, newPath);
}

wxString PerspectiveManager::DoGetPathFromName(const wxString& name)
{
    wxString file;
    wxString filename = name;

    filename.MakeLower();
    if(!filename.EndsWith(wxT(".layout"))) {
        filename << wxT(".layout");
    }

    file << wxStandardPaths::Get().GetUserDataDir() << wxT("/config/") << filename;
    return file;
}

void PerspectiveManager::SavePerspectiveIfNotExists(const wxString& name)
{
    wxString file = DoGetPathFromName(name);
    if(!wxFileName::FileExists(file)) {
        SavePerspective(name, false);
    }
}

bool PerspectiveManager::IsDefaultActive() const
{
    return GetActive().CmpNoCase( wxT("Default") ) == 0;
}

void PerspectiveManager::DoEnsureDebuggerPanesAreVisible()
{
    bool needUpdate = false;
    // First time, make sure that the debugger pane is visible
    wxAuiPaneInfo &info = clMainFrame::Get()->GetDockingManager().GetPane(wxT("Debugger"));
    if(info.IsOk() && !info.IsShown()) {
        info.Show();
        needUpdate = true;
    }
#ifndef __WXMSW__
    wxAuiPaneInfo &dbgPaneInfo = clMainFrame::Get()->GetDockingManager().GetPane(wxT("Debugger Console"));
    if(dbgPaneInfo.IsOk() && !dbgPaneInfo.IsShown()) {
        dbgPaneInfo.Show();
        needUpdate = true;
    }
#endif
    if(needUpdate)
        clMainFrame::Get()->GetDockingManager().Update();
}

void PerspectiveManager::OnPaneClosing(wxAuiManagerEvent& event)
{
    event.Skip();
    if ( !m_aui )
        return;
    
    wxAuiPaneInfo &pane_info = m_aui->GetPane(wxT("Output View"));
    if ( pane_info.IsOk() && pane_info.IsShown() ) {
        // keep the last perspective where the output view 
        // was visible
        m_buildPerspective = m_aui->SavePerspective();
    }
}

void PerspectiveManager::ConnectEvents(wxAuiManager *mgr)
{
    m_aui = mgr;
    if ( m_aui ) {
        mgr->Connect(wxEVT_AUI_PANE_BUTTON, wxAuiManagerEventHandler(PerspectiveManager::OnPaneClosing), NULL, this);
    }
}

void PerspectiveManager::ToggleOutputPane(bool hide)
{
    if ( !m_aui )
        return;
    
    wxAuiPaneInfo &pane_info = m_aui->GetPane(wxT("Output View"));
    
    if ( pane_info.IsOk() && pane_info.IsShown() ) {
        // keep the last perspective where the output view 
        // was visible
        m_buildPerspective = m_aui->SavePerspective();
    }
    
    if ( pane_info.IsOk() && hide && pane_info.IsShown() ) {
        pane_info.Hide();
        m_aui->Update();
        
    } else if ( pane_info.IsOk() && !hide && !pane_info.IsShown() ) {
        if ( !m_buildPerspective.IsEmpty() ) {
            m_aui->LoadPerspective(m_buildPerspective, true);
            
        } else {
            // Else just show it
            pane_info.Show();
            m_aui->Update();
        }
    }

}
void PerspectiveManager::DisconnectEvents()
{
    if ( m_aui ) {
        m_aui->Disconnect(wxEVT_AUI_PANE_BUTTON, wxAuiManagerEventHandler(PerspectiveManager::OnPaneClosing), NULL, this);
    }
    m_aui = NULL;
}

