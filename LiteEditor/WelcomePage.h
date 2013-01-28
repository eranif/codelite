#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H
#include "wxcrafter.h"
#include <map>

class WelcomePage : public WelcomePageBase
{
    typedef std::map<int, wxString> IntStringMap_t;
    IntStringMap_t m_idToName;

protected:
    virtual void OnRecentFileUI(wxUpdateUIEvent& event);
    virtual void OnRecentProjectUI(wxUpdateUIEvent& event);
    int DoGetPopupMenuSelection( wxCommandLinkButton* btn, const wxArrayString& strings, const wxString &menuTitle);

public:
    WelcomePage(wxWindow* parent);
    virtual ~WelcomePage();
protected:
    virtual void OnShowFileseMenu(wxCommandEvent& event);
    virtual void OnShowWorkspaceMenu(wxCommandEvent& event);
    virtual void OnOpenWorkspace(wxCommandEvent& event);
    virtual void OnSize(wxSizeEvent& event);
    virtual void OnNewProject(wxCommandEvent& event);
    virtual void OnOpenForums(wxCommandEvent& event);
    virtual void OnOpenWiki(wxCommandEvent& event);
};
#endif // WELCOMEPAGE_H
