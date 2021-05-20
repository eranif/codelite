//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : qmakesettingsdlg.h
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

#ifndef __qmakesettingsdlg__
#define __qmakesettingsdlg__

#include "qmakesettingsbasedlg.h"

class QmakeConf;
class IManager;

/** Implementing QMakeSettingsBaseDlg */
class QMakeSettingsDlg : public QMakeSettingsBaseDlg
{
    IManager* m_manager;
    QmakeConf* m_conf;
    long m_rightClickTabIdx;

protected:
    // Handlers for QMakeSettingsBaseDlg events.
    void OnNewQmakeSettings(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnRightDown(wxMouseEvent& event);
    void OnRename(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);

    void Initialize();

public:
    /** Constructor */
    QMakeSettingsDlg(wxWindow* parent, IManager* manager, QmakeConf* conf);
    virtual ~QMakeSettingsDlg();
};

#endif // __qmakesettingsdlg__
