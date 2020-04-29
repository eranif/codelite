//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : breakpointpropertiesdlg.h
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

#ifndef __breakptpropertiesdlg__
#define __breakptpropertiesdlg__

#include "breakpointdlgbase.h"
#include "debugger.h"

/** Implementing BreakpointPropertiesDlg */
class BreakptPropertiesDlg : public BreakpointPropertiesDlgBase
{
protected:
    void EndModal(int retCode);
    // Handlers for BreakptPropertiesDlg events.
    void OnCheckBreakLineno(wxCommandEvent& event);
    void OnCheckBreakFunction(wxCommandEvent& event);
    void OnCheckBreakMemory(wxCommandEvent& event);
    void OnBrowse(wxCommandEvent& event);
    void OnCheckBreakLinenoUI(wxUpdateUIEvent& event);
    void OnCheckBreakFunctionUI(wxUpdateUIEvent& event);
    void OnCheckBreakMemoryUI(wxUpdateUIEvent& event);
    void OnPageChanging(wxChoicebookEvent& event);
    void OnPageChanged(wxChoicebookEvent& event);

    bool its_a_breakpt; // Holds whether it's a break or a watch. Used for UpdateUI
    enum whichbreakchk { wbc_line, wbc_function, wbc_memory };
    enum whichbreakchk
        whichBreakcheck; // Holds which of lineno, function or memory checks should be ticked. Used for UpdateUI

public:
    BreakptPropertiesDlg(wxWindow* parent);
    ~BreakptPropertiesDlg();

    /**
     * \brief Insert the data from an existing breakpoint into the dialog fields
     * \param parent
     * \param bp
     */
    void EnterBPData(const clDebuggerBreakpoint& bp);

    clDebuggerBreakpoint b; // Used to return the amended settings
};

#endif // __breakptpropertiesdlg__
