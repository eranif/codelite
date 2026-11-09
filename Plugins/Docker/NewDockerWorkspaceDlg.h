#pragma once
#include "UI.hpp"

#include <wx/filename.h>

class NewDockerWorkspaceDlg : public NewDockerWorkspaceDlgBase
{
public:
    NewDockerWorkspaceDlg(wxWindow* parent);
    virtual ~NewDockerWorkspaceDlg() = default;
    wxFileName GetWorkspaceFile() const;

protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
};
