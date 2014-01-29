//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __GitFileDiffDlg__
#define __GitFileDiffDlg__

#include "gitui.h"

class GitFileDiffDlg : public GitFileDiffDlgBase
{
public:
    GitFileDiffDlg(wxWindow* parent);
    ~GitFileDiffDlg();
    void SetDiff(const wxString& diff);
protected:
    virtual void OnCloseDialog(wxCommandEvent& event);
    virtual void OnSaveAsPatch(wxCommandEvent& event);
};

#endif //__GitFileDiffDlg__
