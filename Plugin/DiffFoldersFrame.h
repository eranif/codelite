#ifndef DIFFFOLDERSFRAME_H
#define DIFFFOLDERSFRAME_H

#include "DiffUI.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK DiffFoldersFrame : public DiffFoldersBaseFrame
{
    wxString m_leftFolder;
    wxString m_rightFolder;

public:
    DiffFoldersFrame(wxWindow* parent);
    virtual ~DiffFoldersFrame();

protected:
    void BuildTrees(const wxString& left, const wxString& right);

protected:
    virtual void OnClose(wxCommandEvent& event);
    virtual void OnNewCmparison(wxCommandEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnItemContextMenu(wxDataViewEvent& event);
};
#endif // DIFFFOLDERSFRAME_H
