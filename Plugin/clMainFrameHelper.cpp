#include "clMainFrameHelper.h"

#include "../LiteEditor/frame.h" // this is daengrous. Only access inline functions
#include "clDockingManager.h"
#include "clStatusBar.h"

#include <wx/frame.h>

clMainFrameHelper::clMainFrameHelper(clMainFrame* frame, clDockingManager* dockMgr)
    : m_mainFrame(frame)
    , m_mgr(dockMgr)
    , m_debuggerFeatures(clDebugEvent::kAllFeatures)
{
}

clMainFrameHelper::~clMainFrameHelper() {}

bool clMainFrameHelper::IsToolbarShown() const { return m_mainFrame && m_mainFrame->GetMainToolBar()->IsShown(); }

bool clMainFrameHelper::IsCaptionsVisible() const
{
    wxAuiPaneInfoArray& panes = m_mgr->GetAllPanes();
    for(size_t i = 0; i < panes.GetCount(); ++i) {
        if(panes.Item(i).IsOk() && !panes.Item(i).IsToolbar()) {
            if(panes.Item(i).HasCaption()) {
                return true;
            }
        }
    }
    return false;
}

bool clMainFrameHelper::IsStatusBarVisible() const { return m_mainFrame && m_mainFrame->GetStatusBar()->IsShown(); }
