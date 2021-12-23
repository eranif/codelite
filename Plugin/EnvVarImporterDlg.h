//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : EnvVarImporterDlg.h
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

#ifndef ENVVARIMPORTERDLG_H
#define ENVVARIMPORTERDLG_H

#include "WSImporterDlgs.h"
#include "workspace.h"

#include <set>

class EnvVarImporterDlg : public EnvVarImporterDlgBase
{
public:
    EnvVarImporterDlg(wxWindow* parent, const wxString& projectName, const wxString& cfgName,
                      std::set<wxString> listEnvVar, BuildConfigPtr le_conf, bool* showDlg);
    virtual ~EnvVarImporterDlg();

protected:
    virtual void OnImport(wxCommandEvent& event);
    virtual void OnContinue(wxCommandEvent& event);
    virtual void OnSkip(wxCommandEvent& event);

private:
    BuildConfigPtr le_conf;
    bool* showDlg;
};

#endif // ENVVARIMPORTERDLG_H
