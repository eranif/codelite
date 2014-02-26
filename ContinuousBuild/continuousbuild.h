#ifndef __ContinuousBuild__
#define __ContinuousBuild__

#include "plugin.h"
#include "buildprocess.h"
#include "compiler.h"
#include "cl_command_event.h"

class wxEvtHandler;
class ContinousBuildPane;
class ShellCommand;

class ContinuousBuild : public IPlugin
{
    ContinousBuildPane *m_view;
    wxEvtHandler *      m_topWin;
    BuildProcess        m_buildProcess;
    wxArrayString       m_files;
    bool                m_buildInProgress;

public:
    void        DoBuild(const wxString &fileName);

public:
    ContinuousBuild(IManager *manager);
    ~ContinuousBuild();

    //--------------------------------------------
    //Abstract methods
    //--------------------------------------------
    virtual clToolBar *CreateToolBar(wxWindow *parent);
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);
    virtual void HookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnPlug();

    void StopAll();

    // Event handlers
    DECLARE_EVENT_TABLE()

    void OnFileSaved           (clCommandEvent &e);
    void OnIgnoreFileSaved     (wxCommandEvent &e);
    void OnStopIgnoreFileSaved (wxCommandEvent &e);
    void OnBuildProcessEnded   (wxCommandEvent &e);
    void OnBuildProcessOutput  (wxCommandEvent &e);
};

#endif //ContinuousBuild
