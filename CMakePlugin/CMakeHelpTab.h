//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CMakeHelpTab.h
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

#ifndef CMAKE_HELP_TAB_H_
#define CMAKE_HELP_TAB_H_

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// C++
#include <map>

// wxWidgets
#include <wx/thread.h>

// UI
#include "CMake.h"
#include "CMakePluginUi.h"

/* ************************************************************************ */
/* FORWARD DECLARATIONS                                                     */
/* ************************************************************************ */

class CMakePlugin;

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

/**
 * @brief Dockable window with CMake help.
 */
class CMakeHelpTab : public CMakeHelpTabBase, public wxThreadHelper, public CMake::LoadNotifier
{

    // Public Ctors & Dtors
public:
    /**
     * @brief Constructor.
     *
     * @param parent Pointer to parent window.
     * @param plugin Pointer to the plugin.
     */
    CMakeHelpTab(wxWindow* parent, CMakePlugin* plugin);

    // Protected Events
protected:
    /**
     * @brief create help page with a given content
     */
    void CreateHelpPage(const wxString& content, const wxString& subject);

    /**
     * @brief On topic change.
     *
     * @param event
     */
    virtual void OnChangeTopic(wxCommandEvent& event);

    /**
     * @brief On item insert into editor.
     *
     * @param event
     */
    virtual void OnInsert(wxCommandEvent& event);

    /**
     * @brief On item search.
     *
     * @param event
     */
    virtual void OnSearch(wxCommandEvent& event);

    /**
     * @brief On item search cancel.
     *
     * @param event
     */
    virtual void OnSearchCancel(wxCommandEvent& event);

    /**
     * @brief On item select.
     *
     * @param event
     */
    virtual void OnSelect(wxCommandEvent& event);

    /**
     * @brief On CMake help data reload.
     *
     * @param event
     */
    virtual void OnReload(wxCommandEvent& event);

    /**
     * @brief Notify about loading start.
     *
     * @param event
     */
    void OnThreadStart(wxThreadEvent& event);

    /**
     * @brief Updates gauge by current state of the background thread.
     *
     * @param event
     */
    void OnThreadUpdate(wxThreadEvent& event);

    /**
     * @brief Notify about loading is done
     *
     * @param event
     */
    void OnThreadDone(wxThreadEvent& event);

    /**
     * @brief On tab close.
     *
     * @param event
     */
    void OnClose(wxCloseEvent& event);

    /**
     * @brief Some items update UI.
     *
     * @param event
     */
    virtual void OnUpdateUi(wxUpdateUIEvent& event);

    // Public Operations
public:
    /**
     * @brief Show required topic.
     *
     * @param topic Topic number.
     */
    void ShowTopic(int topic);

    /**
     * @brief Show CMake help data in UI.
     */
    void PublishData();

    // CMake::LoadNotifier
public:
    /**
     * @brief Checks if loading should be stopped.
     *
     * @return
     */
    virtual bool RequestStop() const;

    /**
     * @brief Loading is started.
     */
    virtual void Start();

    /**
     * @brief Update loading progress.
     *
     * @param value Value is in range [0, 100].
     */
    virtual void Update(int value);

    /**
     * @brief Increase loading progress.
     *
     * @param value
     */
    virtual void Inc(int value);

    /**
     * @brief Loading is done.
     */
    virtual void Done();

    /**
     * @brief stop the worker thread
     */
    virtual void Stop();

    // Protected Operations
protected:
    /**
     * @brief Does the thread things.
     *
     * @return Exit code.
     */
    virtual wxThread::ExitCode Entry();

    /**
     * @brief Loads data from CMake object into UI objects.
     *
     * @param force If data should be reloaded from CMake instead
     *              from database.
     */
    void LoadData(bool force = false);

    // Private Operations
private:
    /**
     * @brief List all items.
     */
    void ListAll();

    /**
     * @brief List only items that match search string.
     *
     * @param search
     */
    void ListFiltered(const wxString& search);

    // Private Data Members
private:
    /// A pointer to cmake plugin.
    CMakePlugin* const m_plugin;

    /// Current topic data.
    const std::map<wxString, wxString>* m_data;

    /// Temporary variable.
    bool m_force;

    /// Current progress state.
    int m_progress;
};

/* ************************************************************************ */

#endif // CMAKE_HELP_TAB_H_
