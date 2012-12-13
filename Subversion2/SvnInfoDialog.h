#ifndef SVNINFODIALOG_H
#define SVNINFODIALOG_H
#include "subversion2_ui.h"

class SvnInfoDialog : public SvnInfoDialogBase
{
public:
    SvnInfoDialog(wxWindow* parent);
    virtual ~SvnInfoDialog();
    wxTextCtrl* GetTextCtrlAuthor() {
        return m_textCtrlAuthor;
    }
    wxTextCtrl* GetTextCtrlDate() {
        return m_textCtrlDate;
    }
    wxTextCtrl* GetTextCtrlRevision() {
        return m_textCtrlRevision;
    }
    wxTextCtrl* GetTextCtrlRootURL() {
        return m_textCtrlRootURL;
    }
    wxTextCtrl* GetTextCtrlURL() {
        return m_textCtrlURL;
    }
};
#endif // SVNINFODIALOG_H
