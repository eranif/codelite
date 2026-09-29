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

#include "editorsettingslocalbase.hpp"
#include "globals.h"
#include "localworkspace.h"
#include "optionsconfig.h"

class EditorSettingsLocal : public LocalEditorSettingsbase
{
public:
    EditorSettingsLocal(OptionsConfigPtr higherOptions,
                        wxXmlNode* node,
                        enum prefsLevel level = pLevel_dunno,
                        wxWindow* parent = nullptr,
                        wxWindowID id = wxID_ANY,
                        const wxString& title = _("Local Preferences"));
    ~EditorSettingsLocal() override = default;

    LocalOptionsConfigPtr GetLocalOpts() const { return localOptions; }

protected:
    void DisplayHigherValues(const OptionsConfigPtr options);
    void DisplayLocalValues(const LocalOptionsConfigPtr options);

    void indentsUsesTabsUpdateUI(wxUpdateUIEvent& event) override;
    void indentWidthUpdateUI(wxUpdateUIEvent& event) override;
    void tabWidthUpdateUI(wxUpdateUIEvent& event) override;
    void displayBookmarkMarginUpdateUI(wxUpdateUIEvent& event) override;
    void checkBoxDisplayFoldMarginUpdateUI(wxUpdateUIEvent& event) override;
    void checkBoxHideChangeMarkerMarginUpdateUI(wxUpdateUIEvent& event) override;
    void displayLineNumbersUpdateUI(wxUpdateUIEvent& event) override;
    void showIndentationGuideLinesUpdateUI(wxUpdateUIEvent& event) override;
    void highlightCaretLineUpdateUI(wxUpdateUIEvent& event) override;
    void checkBoxTrimLineUpdateUI(wxUpdateUIEvent& event) override;
    void checkBoxAppendLFUpdateUI(wxUpdateUIEvent& event) override;
    void whitespaceStyleUpdateUI(wxUpdateUIEvent& event) override;
    void choiceEOLUpdateUI(wxUpdateUIEvent& event) override;
    void fileEncodingUpdateUI(wxUpdateUIEvent& event) override;

    void OnOK(wxCommandEvent& event) override;

    StringManager m_EOLstringManager;
    StringManager m_WSstringManager;
    LocalOptionsConfigPtr localOptions;
    OptionsConfigPtr higherOptions;
    wxXmlNode* node;
};
#endif // __editorsettingslocal__
