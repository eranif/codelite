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
#include "CMakeHelpPanel.h"

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

CMakeHelpPanel::CMakeHelpPanel(wxWindow* parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size,
                               long style)
    : CMakeHelpPanelBase(parent, id, pos, size, style)
{
    // Nothing to do
}

/* ************************************************************************ */

CMakeHelpPanel::~CMakeHelpPanel()
{
    // Nothing to do
}

/* ************************************************************************ */

void
CMakeHelpPanel::SetData(const std::map<wxString, wxString>* data)
{
    m_data = data;

    // List all items
    ListAll();
}

/* ************************************************************************ */

void
CMakeHelpPanel::OnSelect(wxCommandEvent& event)
{
    wxASSERT(m_data);

    // Get selected name
    const wxString name = m_listBoxList->GetString(event.GetInt());

    // Find name in the data
    std::map<wxString, wxString>::const_iterator it = m_data->find(name);

    // Data found
    if (it != m_data->end()) {
        // Show required data
        m_htmlWinText->SetPage(it->second);
    }
}

/* ************************************************************************ */

void
CMakeHelpPanel::OnSearch(wxCommandEvent& event)
{
    // List subset
    ListFiltered(event.GetString());
}

/* ************************************************************************ */

void
CMakeHelpPanel::OnSearchCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);

    // List all items
    ListAll();
}

/* ************************************************************************ */

void
CMakeHelpPanel::ListAll()
{
    // Remove old data
    m_listBoxList->Clear();
    m_htmlWinText->SetPage("");

    // Foreach data and store names into list
    for (std::map<wxString, wxString>::const_iterator it = m_data->begin(),
         ite = m_data->end(); it != ite; ++it) {
        m_listBoxList->Append(it->first);
    }
}

/* ************************************************************************ */

void
CMakeHelpPanel::ListFiltered(const wxString& search)
{
    const wxString searchMatches = "*" + search + "*";

    // Remove old data
    m_listBoxList->Clear();
    m_htmlWinText->SetPage("");

    // Foreach data and store names into list
    for (std::map<wxString, wxString>::const_iterator it = m_data->begin(),
         ite = m_data->end(); it != ite; ++it) {
        // Store only that starts with given string
        if (it->first.Matches(searchMatches))
            m_listBoxList->Append(it->first);
    }
}

/* ************************************************************************ */
