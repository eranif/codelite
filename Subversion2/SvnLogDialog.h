#ifndef SVNLOGDIALOG_H
#define SVNLOGDIALOG_H
#include "subversion2_ui.h"

class SvnLogDialog : public SvnLogDialogBase
{
public:
    SvnLogDialog(wxWindow* parent);
    virtual ~SvnLogDialog();

    void SetCompact(wxCheckBox* compact) {
        this->m_compact = compact;
    }
    void SetFrom(wxTextCtrl* from) {
        this->m_from = from;
    }
    void SetTo(wxTextCtrl* to) {
        this->m_to = to;
    }
    wxCheckBox* GetCompact() {
        return m_compact;
    }
    wxTextCtrl* GetFrom() {
        return m_from;
    }
    wxTextCtrl* GetTo() {
        return m_to;
    }
};
#endif // SVNLOGDIALOG_H
