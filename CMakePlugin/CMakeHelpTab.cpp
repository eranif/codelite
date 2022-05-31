//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CMakeHelpTab.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/* ************************************************************************ */
/*                                                                          */
/* CMakePlugin for Codelite                                                 */
/* Copyright (C) 2013 Jiří Fatka <ntsfka@gmail.com>                         */
/*                                                                          */
/* This program is free software: you can redistribute it and/or modify     */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* This program is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with this program. If not, see <http://www.gnu.org/licenses/>.     */
/*                                                                          */
/* ************************************************************************ */

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// Declaration
#include "CMakeHelpTab.h"

#include "ColoursAndFontsManager.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"

#include <wx/filename.h>
#include <wx/stc/stc.h>

// wxWidgets
#include <wx/busyinfo.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>

// Codelite
#include "file_logger.h"
#include "ieditor.h"
#include "imanager.h"

// CMakePlugin
#include "CMake.h"
#include "CMakePlugin.h"

/* ************************************************************************ */
/* DEFINITIONS                                                              */
/* ************************************************************************ */

wxDEFINE_EVENT(EVT_THREAD_START, wxThreadEvent);
wxDEFINE_EVENT(EVT_THREAD_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(EVT_THREAD_DONE, wxThreadEvent);

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

CMakeHelpTab::CMakeHelpTab(wxWindow* parent, CMakePlugin* plugin)
    : CMakeHelpTabBase(parent)
    , m_plugin(plugin)
    , m_force(false)
{
    wxASSERT(plugin);
    wxASSERT(m_gaugeLoad->GetRange() == 100); // Must be 100

    Bind(wxEVT_CLOSE_WINDOW, &CMakeHelpTab::OnClose, this);
    Bind(EVT_THREAD_START, &CMakeHelpTab::OnThreadStart, this);
    Bind(EVT_THREAD_UPDATE, &CMakeHelpTab::OnThreadUpdate, this);
    Bind(EVT_THREAD_DONE, &CMakeHelpTab::OnThreadDone, this);
}

/* ************************************************************************ */

void CMakeHelpTab::OnChangeTopic(wxCommandEvent& event) { ShowTopic(event.GetInt()); }

/* ************************************************************************ */

void CMakeHelpTab::OnInsert(wxCommandEvent& event)
{
    IManager* manager = m_plugin->GetManager();
    wxASSERT(manager);
    IEditor* editor = manager->GetActiveEditor();

    // No active editor
    if(!editor)
        return;

    // Insert value
    editor->InsertText(editor->GetCurrentPosition(), m_listBoxList->GetString(event.GetInt()));
}

/* ************************************************************************ */

void CMakeHelpTab::OnSearch(wxCommandEvent& event)
{
    // List subset
    ListFiltered(event.GetString());
}

/* ************************************************************************ */

void CMakeHelpTab::OnSearchCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);

    // List all items
    ListAll();
}

/* ************************************************************************ */

void CMakeHelpTab::OnSelect(wxCommandEvent& event)
{
    wxASSERT(!GetThread() || !GetThread()->IsRunning());
    wxASSERT(m_data);

    // Get selected name
    const wxString name = m_listBoxList->GetString(event.GetInt());

    // Find name in the data
    std::map<wxString, wxString>::const_iterator it = m_data->find(name);

    // Data found
    if(it != m_data->end()) {
        CreateHelpPage(it->second, name);
    }
}

/* ************************************************************************ */

void CMakeHelpTab::OnReload(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxASSERT(m_plugin->GetCMake());
    if(!m_plugin->GetCMake()->IsOk()) {
        wxMessageBox(_("CMake application path is invalid!"), wxMessageBoxCaptionStr, wxOK | wxCENTER | wxICON_ERROR);
        return;
    }

    LoadData(true);
}

/* ************************************************************************ */

void CMakeHelpTab::ListAll()
{
    // Remove old data
    m_listBoxList->Clear();

    if(!m_data)
        return;

    // Foreach data and store names into list
    for(std::map<wxString, wxString>::const_iterator it = m_data->begin(), ite = m_data->end(); it != ite; ++it) {
        m_listBoxList->Append(it->first);
    }
}

/* ************************************************************************ */

void CMakeHelpTab::ListFiltered(const wxString& search)
{
    const wxString searchMatches = "*" + search + "*";

    // Remove old data
    m_listBoxList->Clear();

    if(!m_data)
        return;

    // Foreach data and store names into list
    for(std::map<wxString, wxString>::const_iterator it = m_data->begin(), ite = m_data->end(); it != ite; ++it) {
        // Store only that starts with given string
        if(it->first.Matches(searchMatches))
            m_listBoxList->Append(it->first);
    }
}

/* ************************************************************************ */

/* ************************************************************************ */

void CMakeHelpTab::OnThreadStart(wxThreadEvent& event)
{
    // Show gauge
    if(!m_gaugeLoad->IsShown()) {
        m_gaugeLoad->Show();
        Layout();
    }
}

/* ************************************************************************ */

void CMakeHelpTab::OnThreadUpdate(wxThreadEvent& event)
{
    // Notify about update
    m_gaugeLoad->SetValue(event.GetInt());
    m_gaugeLoad->Update();
}

/* ************************************************************************ */

void CMakeHelpTab::OnThreadDone(wxThreadEvent& event)
{
    // Hide gauge
    m_gaugeLoad->Hide();
    Layout();

    // Publish loaded data
    PublishData();
}

/* ************************************************************************ */

void CMakeHelpTab::OnClose(wxCloseEvent& event)
{
    // Wait for thread
    if(GetThread() && GetThread()->IsRunning())
        GetThread()->Wait();

    Destroy();
}

/* ************************************************************************ */

void CMakeHelpTab::OnUpdateUi(wxUpdateUIEvent& event)
{
    // Disable UI when background thread is running
    event.Enable(!GetThread() || !GetThread()->IsRunning());
}

/* ************************************************************************ */

void CMakeHelpTab::ShowTopic(int topic)
{
    wxASSERT(!GetThread() || !GetThread()->IsRunning());

    const CMake* cmake = m_plugin->GetCMake();
    wxASSERT(cmake);

    switch(topic) {
    default:
        m_data = NULL;
        break;

    case 0:
        m_data = &cmake->GetModules();
        break;

    case 1:
        m_data = &cmake->GetCommands();
        break;

    case 2:
        m_data = &cmake->GetVariables();
        break;

    case 3:
        m_data = &cmake->GetProperties();
        break;
    }

    // Clear filter
    m_searchCtrlFilter->Clear();

    // List all items
    ListAll();
}

/* ************************************************************************ */

void CMakeHelpTab::PublishData()
{
    // The background thread must not working now
    if(GetThread() && GetThread()->IsRunning())
        return;

    // Set CMake version
    m_staticTextVersionValue->SetLabel(m_plugin->GetCMake()->GetVersion());

    // Show the first topic
    ShowTopic(0);
}

/* ************************************************************************ */

wxThread::ExitCode CMakeHelpTab::Entry()
{
    CMake* cmake = m_plugin->GetCMake();
    wxASSERT(cmake);

    // Load data
    cmake->LoadData(m_force, this);

    return static_cast<wxThread::ExitCode>(0);
}

/* ************************************************************************ */

void CMakeHelpTab::LoadData(bool force)
{
    // Thread is busy
    if(GetThread() && GetThread()->IsRunning()) {
        return;
    }

    // Invalid cmake executable
    wxASSERT(m_plugin->GetCMake());
    if(!m_plugin->GetCMake()->IsOk()) {
        return;
    }

    m_force = force;

    // Create a new joinable thread
    if(CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR) {
        clERROR() << "Could not create the worker thread!" << endl;
        return;
    }

    // For sure :)
    wxASSERT(GetThread());

    // Run the thread
    if(GetThread()->Run() != wxTHREAD_NO_ERROR) {
        clERROR() << "Could not run the worker thread!" << endl;
        return;
    }
}

/* ************************************************************************ */

bool CMakeHelpTab::RequestStop() const { return GetThread() && GetThread()->TestDestroy(); }

/* ************************************************************************ */

void CMakeHelpTab::Start() { AddPendingEvent(wxThreadEvent(EVT_THREAD_START)); }

/* ************************************************************************ */

void CMakeHelpTab::Update(int value)
{
    m_progress = value;

    // It safe to use stack version because we don't use wxString value.
    wxThreadEvent event(EVT_THREAD_UPDATE);
    event.SetInt(value);
    AddPendingEvent(event);
}

/* ************************************************************************ */

void CMakeHelpTab::Inc(int value)
{
    // There is nothing to add
    if(!value)
        return;

    Update(m_progress + value);
}

/* ************************************************************************ */

void CMakeHelpTab::Done() { AddPendingEvent(wxThreadEvent(EVT_THREAD_DONE)); }

void CMakeHelpTab::Stop()
{
    if(GetThread() && GetThread()->IsAlive()) {
        GetThread()->Delete(NULL, wxTHREAD_WAIT_BLOCK);

    } else if(GetThread()) {
        GetThread()->Wait(wxTHREAD_WAIT_BLOCK);
    }
}

void CMakeHelpTab::CreateHelpPage(const wxString& content, const wxString& subject)
{
    wxString text = content;
    text.Replace("<br />", "\n");
    text.Replace("&lt;", "<");
    text.Replace("&gt;", ">");
    text.Replace("\r", "");
    text.Replace("\n\n", "\n");
    text.Replace("::\n", "\n\n");
    IManager* manager = ::clGetManager();

    // Write the content of the help into a temporary file
    wxFileName fnTemp = wxFileName::CreateTempFileName("cmake");
    wxFileName fnCMakeHelpFile = fnTemp;
    fnCMakeHelpFile.SetFullName("CMakeHelp.cmake");

    if(!FileUtils::WriteFileContent(fnCMakeHelpFile, text))
        return;

    if(manager->OpenFile(fnCMakeHelpFile.GetFullPath())) {
        IEditor* activeEditor = manager->GetActiveEditor();
        if(activeEditor && activeEditor->GetFileName().GetFullPath() == fnCMakeHelpFile.GetFullPath()) {
            activeEditor->GetCtrl()->SetEditable(true);
            activeEditor->OpenFile();
            activeEditor->GetCtrl()->SetFirstVisibleLine(0);
            activeEditor->GetCtrl()->SetEditable(false);
        }
    }
}

/* ************************************************************************ */
