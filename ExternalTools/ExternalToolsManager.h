#ifndef EXTERNALTOOLSMANAGER_H
#define EXTERNALTOOLSMANAGER_H
#include "external_tools.h"

class ExternalToolsManager : public ExternalToolsManagerBase
{
public:
    ExternalToolsManager(wxWindow* parent);
    virtual ~ExternalToolsManager();

protected:
    virtual void OnRefresh(wxCommandEvent& event);
    void DoPopulateTable();
    void DoClear();

protected:
    virtual void OnKill(wxCommandEvent& event);
    virtual void OnKillAll(wxCommandEvent& event);
    virtual void OnKillAllUI(wxUpdateUIEvent& event);
    virtual void OnKillUI(wxUpdateUIEvent& event);
};
#endif // EXTERNALTOOLSMANAGER_H
