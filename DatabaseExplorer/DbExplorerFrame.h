#ifndef DBEXPLORERFRAME_H
#define DBEXPLORERFRAME_H
#include "GUI.h"

class DbViewerPanel;
class DbExplorerFrame : public DbExplorerFrameBase
{
    DbViewerPanel* m_viewer;

protected:
    void OnClose(wxCloseEvent& event);

public:
    DbExplorerFrame(wxWindow* parent, wxWindow* page, const wxString& label, DbViewerPanel* viewer);
    virtual ~DbExplorerFrame();
};
#endif // DBEXPLORERFRAME_H
