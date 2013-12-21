//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __gitSettingsDlg__
#define __gitSettingsDlg__

#include "gitui.h"
#include "git.h"

class GitSettingsDlg : public GitSettingsDlgBase
{
    wxString m_localRepoPath;
public:
    GitSettingsDlg(wxWindow* parent, const wxString& localRepoPath);
    virtual ~GitSettingsDlg();

protected:
    virtual void OnLocalRepoUI(wxUpdateUIEvent& event);
    virtual void OnOK(wxCommandEvent& event);
};

#endif //__gitSettingsDlg__
