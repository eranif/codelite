#ifndef NODEJSNEWWORKSPACEDLG_H
#define NODEJSNEWWORKSPACEDLG_H
#include "WebToolsBase.h"

class NodeJSNewWorkspaceDlg : public NodeJSNewWorkspaceDlgBase
{
protected:
    virtual void OnCheckNewFolder(wxCommandEvent& event);
    void UpdatePreview();

public:
    NodeJSNewWorkspaceDlg(wxWindow* parent);
    virtual ~NodeJSNewWorkspaceDlg();
    wxString GetWorkspaceFilename() const { return m_staticTextPreview->GetLabel(); }

protected:
    virtual void OnFolderSelected(wxFileDirPickerEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnTextUpdate(wxCommandEvent& event);
};
#endif // NODEJSNEWWORKSPACEDLG_H
