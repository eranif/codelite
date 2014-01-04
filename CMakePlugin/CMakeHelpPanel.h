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

#ifndef CMAKE_HELP_PANEL_H_
#define CMAKE_HELP_PANEL_H_

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// C++
#include <map>

// wxWidgets
#include <wx/string.h>
#include <wx/arrstr.h>

// UI
#include "CMakeHelpPanelBase.h"

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

/**
 * @brief Panel for help dialog where on the left side is a list of values
 * and on the right side is value description.
 */
class CMakeHelpPanel : public CMakeHelpPanelBase
{

// Public Ctors & Dtors
public:


    /**
     * @brief Constructor.
     *
     * Full list of parameters are required because this class is
     * constructed within wxCrafter's generated code and it calls
     * constructor with full list of the wxPanel parameters (only
     * the last parameter is missing).
     *
     * @param parent The parent window.
     * @param id     An identifier for the panel. wxID_ANY is taken to mean
     *               a default.
     * @param pos    The panel position. The value wxDefaultPosition
     *               indicates a default position, chosen by either the
     *               windowing system or wxWidgets, depending on platform.
     * @param size   The panel size. The value wxDefaultSize indicates
     *               a default size, chosen by either the windowing system
     *               or wxWidgets, depending on platform.
     * @param style  The window style. See wxPanel.
     */
    explicit CMakeHelpPanel(wxWindow* parent, wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxSize(500, 300),
                            long style = wxTAB_TRAVERSAL);


    /**
     * @brief Destructor.
     */
    virtual ~CMakeHelpPanel();


// Public Mutators
public:


    /**
     * @brief Set panel data.
     *
     * Data is displayed on the panel as: keys in the list box and
     * value in the text control after an item in the list box is
     * selected.
     *
     * @param data     A pointer to displayed data.
     */
    void SetData(const std::map<wxString, wxString>* data);


// Public Events
public:


    /**
     * @brief On list item selection.
     *
     * @param event
     */
    virtual void OnSelect(wxCommandEvent& event);


    /**
     * @brief On search item.
     *
     * @param event
     */
    virtual void OnSearch(wxCommandEvent& event);


    /**
     * @brief On search cancel - it list all items.
     *
     * @param event.
     */
    virtual void OnSearchCancel(wxCommandEvent& event);


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


    /// Panel data.
    const std::map<wxString, wxString>* m_data;

};

/* ************************************************************************ */

#endif // CMAKE_HELP_PANEL_H_
