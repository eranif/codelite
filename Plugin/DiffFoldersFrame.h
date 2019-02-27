#ifndef DIFFFOLDERSFRAME_H
#define DIFFFOLDERSFRAME_H
#include "DiffUI.h"

class DiffFoldersFrame : public DiffFoldersBaseFrame
{
public:
    DiffFoldersFrame(wxWindow* parent);
    virtual ~DiffFoldersFrame();

protected:
    void BuildTrees(const wxString& left, const wxString& right);

protected:
    virtual void OnClose(wxCommandEvent& event);
    virtual void OnNewCmparison(wxCommandEvent& event);
    virtual void OnLeftFolderExpanding(wxTreeEvent& event);
    virtual void OnLeftItemActivated(wxTreeEvent& event);
    virtual void OnLeftSelectionChanged(wxTreeEvent& event);
    virtual void OnRightFolderExpanding(wxTreeEvent& event);
    virtual void OnRightItemActivated(wxTreeEvent& event);
    virtual void OnRightSelectionChanged(wxTreeEvent& event);
};
#endif // DIFFFOLDERSFRAME_H
