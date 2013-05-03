#ifndef PROJECTCUSTOMBUILDTRAGETDLG_H
#define PROJECTCUSTOMBUILDTRAGETDLG_H
#include "project_settings_base_dlg.h"

class ProjectCustomBuildTragetDlg : public ProjectCustomBuildTragetDlgBase
{
public:
    static const wxString CUSTOM_TARGET_BUILD               ;
    static const wxString CUSTOM_TARGET_CLEAN               ;
    static const wxString CUSTOM_TARGET_REBUILD             ;
    static const wxString CUSTOM_TARGET_COMPILE_SINGLE_FILE ;
    static const wxString CUSTOM_TARGET_PREPROCESS_FILE     ;

public:
    ProjectCustomBuildTragetDlg(wxWindow* parent, const wxString &targetName, const wxString& targetCommand);
    virtual ~ProjectCustomBuildTragetDlg();

    wxString GetTargetName() const {
        return m_textCtrlTargetName->GetValue();
    }

    wxString GetTargetCommand() const {
        return m_textCtrlCommand->GetValue();
    }
    
    static bool IsPredefinedTarget(const wxString& name);
    
protected:
    virtual void OnEditTargetNameUI(wxUpdateUIEvent& event);
};
#endif // PROJECTCUSTOMBUILDTRAGETDLG_H
