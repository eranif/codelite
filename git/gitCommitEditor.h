//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __gitCommitEditor__
#define __gitCommitEditor__

#include <wx/wx.h>
#include <wx/stc/stc.h>

class GitCommitEditor : public wxStyledTextCtrl
{
public:
    GitCommitEditor(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint &position = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
    void InitStyles();
};

#endif //__gitCommitEditor__
