#ifndef NEWCOMPILERDLG_H
#define NEWCOMPILERDLG_H
#include "compiler_pages.h"

class NewCompilerDlg : public NewCompilerDlgBase
{
public:
    NewCompilerDlg(wxWindow* parent);
    virtual ~NewCompilerDlg();

    wxString GetCompilerName() const {
        return m_textCtrlCompilerName->GetValue();
    }

    wxString GetMasterCompiler() const;
protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
};
#endif // NEWCOMPILERDLG_H
