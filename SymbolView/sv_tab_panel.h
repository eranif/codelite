#ifndef SYMBOLVIEWTABPANEL_H
#define SYMBOLVIEWTABPANEL_H

#include "wxcrafter.h"

class IManager;
class svSymbolTree;

class SymbolViewTabPanel : public SymbolViewTabPanelBaseClass
{
protected:
    IManager* m_mgr;
    svSymbolTree* m_tree;
public:
    SymbolViewTabPanel(wxWindow* parent, IManager* mgr);
    virtual ~SymbolViewTabPanel();
    void OnSearchSymbol(wxCommandEvent& event);
    void OnSearchEnter(wxCommandEvent& event);

    // Event Handlers
    ////////////////////////////////////////////////
    void OnWorkspaceLoaded(wxCommandEvent &e);
    void OnWorkspaceClosed(wxCommandEvent &e);
    void OnActiveEditorChanged(wxCommandEvent &e);
    void OnEditorClosed(wxCommandEvent &e);
    void OnAllEditorsClosed(wxCommandEvent &e);
    void OnFilesTagged(wxCommandEvent &e);
    void OnMenu(wxContextMenuEvent &e);
    void OnGotoImpl(wxCommandEvent &e);
    void OnGotoDecl(wxCommandEvent &e);
    void OnFindReferenes(wxCommandEvent &e);
};

#endif // SYMBOLVIEWTABPANEL_H
