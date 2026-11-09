#pragma once
#include "GUI.hpp"

class DbViewerPanel;
class DbExplorerFrame : public DbExplorerFrameBase
{
    DbViewerPanel* m_viewer;

protected:
    void OnClose(wxCloseEvent& event);

public:
    DbExplorerFrame(wxWindow* parent, wxWindow* page, const wxString& label, DbViewerPanel* viewer);
    ~DbExplorerFrame() override;
};
