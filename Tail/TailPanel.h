#ifndef TAILPANEL_H
#define TAILPANEL_H

#include "TailUI.h"
#include "clFileSystemWatcher.h"
#include <wx/filename.h>
#include <vector>
#include "clFileSystemEvent.h"
#include "clEditorEditEventsHandler.h"

class TailPanel : public TailPanelBase
{
    clFileSystemWatcher::Ptr_t m_fileWatcher;
    wxFileName m_file;
    size_t m_lastPos;
    clEditEventsHandler::Ptr_t m_editEvents;

protected:
    virtual void OnClear(wxCommandEvent& event);
    virtual void OnClearUI(wxUpdateUIEvent& event);
    virtual void OnClose(wxCommandEvent& event);
    virtual void OnCloseUI(wxUpdateUIEvent& event);
    void DoClear();
    void DoAppendText(const wxString& text);

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
    void OnThemeChanged(wxCommandEvent& event);
};
#endif // TAILPANEL_H
