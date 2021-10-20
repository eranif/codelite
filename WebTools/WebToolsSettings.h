//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : WebToolsSettings.h
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

#ifndef WEBTOOLSSETTINGS_H
#define WEBTOOLSSETTINGS_H
#include "WebToolsBase.h"

class WebToolsSettings : public WebToolsSettingsBase
{
    bool m_modified;

public:
    WebToolsSettings(wxWindow* parent);
    virtual ~WebToolsSettings();

private:
    void DoSave();

protected:
    virtual void OnLintOnSave(wxCommandEvent& event);
    virtual void OnApply(wxCommandEvent& event);
    virtual void OnNodejsPath(wxFileDirPickerEvent& event);
    virtual void OnNpmPath(wxFileDirPickerEvent& event);
    virtual void OnSuggestNodeJSPaths(wxCommandEvent& event);
    virtual void OnModified(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // WEBTOOLSSETTINGS_H
