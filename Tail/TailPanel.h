#ifndef TAILPANEL_H
#define TAILPANEL_H

#include "TailUI.h"
#include "clFileSystemWatcher.h"
#include <wx/filename.h>
#include <vector>
#include "clFileSystemEvent.h"

class TailPanel : public TailPanelBase
{
    clFileSystemWatcher::Ptr_t m_fileWatcher;
    wxFileName m_file;

protected:
    void DoClear();

public:
    TailPanel(wxWindow* parent);
    virtual ~TailPanel();

protected:
    virtual void OnOpenFile(wxCommandEvent& event);
    virtual void OnPause(wxCommandEvent& event);
    virtual void OnPauseUI(wxUpdateUIEvent& event);
    virtual void OnPlay(wxCommandEvent& event);
    virtual void OnPlayUI(wxUpdateUIEvent& event);
    void OnFileModified(clFileSystemEvent& event);
};
#endif // TAILPANEL_H
