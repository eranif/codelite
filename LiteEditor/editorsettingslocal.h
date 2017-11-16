//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : editorsettingslocal.h
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

#ifndef __editorsettingslocal__
#define __editorsettingslocal__

#include "editorsettingslocalbase.h"
#include "globals.h"
#include "localworkspace.h"
#include "optionsconfig.h"

class EditorSettingsLocal : public LocalEditorSettingsbase
{
public:
    EditorSettingsLocal(OptionsConfigPtr higherOptions, wxXmlNode* node, enum prefsLevel level = pLevel_dunno,
                        wxWindow* parent = NULL, wxWindowID id = wxID_ANY,
                        const wxString& title = _("Local Preferences"));
    ~EditorSettingsLocal();

    LocalOptionsConfigPtr GetLocalOpts() const { return localOptions; }

protected:
    void DisplayHigherValues(const OptionsConfigPtr options);
    void DisplayLocalValues(const LocalOptionsConfigPtr options);

    void indentsUsesTabsUpdateUI(wxUpdateUIEvent& event);
    void indentWidthUpdateUI(wxUpdateUIEvent& event);
    void tabWidthUpdateUI(wxUpdateUIEvent& event);
    void displayBookmarkMarginUpdateUI(wxUpdateUIEvent& event);
    void checkBoxDisplayFoldMarginUpdateUI(wxUpdateUIEvent& event);
    void checkBoxHideChangeMarkerMarginUpdateUI(wxUpdateUIEvent& event);
    void displayLineNumbersUpdateUI(wxUpdateUIEvent& event);
    void showIndentationGuideLinesUpdateUI(wxUpdateUIEvent& event);
    void highlightCaretLineUpdateUI(wxUpdateUIEvent& event);
    void checkBoxTrimLineUpdateUI(wxUpdateUIEvent& event);
    void checkBoxAppendLFUpdateUI(wxUpdateUIEvent& event);
    void whitespaceStyleUpdateUI(wxUpdateUIEvent& event);
    void choiceEOLUpdateUI(wxUpdateUIEvent& event);
    void fileEncodingUpdateUI(wxUpdateUIEvent& event);

    void OnOK(wxCommandEvent& event);

    StringManager m_EOLstringManager;
    StringManager m_WSstringManager;
    LocalOptionsConfigPtr localOptions;
    OptionsConfigPtr higherOptions;
    wxXmlNode* node;
};
#endif // __editorsettingslocal__
