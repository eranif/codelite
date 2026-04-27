#pragma once

#include "clEditorEditEventsHandler.h"
#include "clFileSystemWorkspaceDlgBase.h"

class BuildTargetDlg : public BuildTargetDlgBase
{
public:
    BuildTargetDlg(wxWindow* parent, const wxString& name, const wxString& command);
    ~BuildTargetDlg() override;

    wxString GetTargetName() const;
    wxString GetTargetCommand() const;

protected:
    void OnOK_UI(wxUpdateUIEvent& event) override;

private:
    std::unique_ptr<clEditEventsHandler> m_editHelper{nullptr};
};
