//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __gitSettingsDlg__
#define __gitSettingsDlg__

#include "gitui.h"

class GitSettingsDlg : public GitSettingsDlgBase
{
public:
	GitSettingsDlg(wxWindow* parent,const wxColour& tracked, const wxColour& diff,
	               const wxString& pathGIT, const wxString& pathGITK);
	const wxColour GetTrackedFileColour();
	const wxColour GetDiffFileColour();
	const wxString GetGITExecutablePath();
	const wxString GetGITKExecutablePath();
};

#endif //__gitSettingsDlg__
