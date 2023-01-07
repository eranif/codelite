#ifndef WXCAUIMANAGER_H
#define WXCAUIMANAGER_H

#include <map>
#include <wx/aui/framemanager.h>

class wxcAuiManager
{
protected:
    std::map<wxWindow*, wxAuiManager*> m_auiMgrMap;

private:
    wxcAuiManager();
    virtual ~wxcAuiManager();

public:
    static wxcAuiManager& Get();

    void UnInit(wxWindow* win);
    void Add(wxWindow* win, wxAuiManager* aui);

    wxAuiManager* Find(wxWindow* win) const;
};

#endif // WXCAUIMANAGER_H
