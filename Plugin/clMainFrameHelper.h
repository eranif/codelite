#ifndef CLMAINFRAMEHELPER_H
#define CLMAINFRAMEHELPER_H

#include "codelite_exports.h"
#include "smart_ptr.h"

#include <wx/event.h>

class clMainFrame;
class clDockingManager;

class WXDLLIMPEXP_SDK clMainFrameHelper : public wxEvtHandler
{
    clMainFrame* m_mainFrame;
    clDockingManager* m_mgr;
    size_t m_debuggerFeatures;

public:
    typedef SmartPtr<clMainFrameHelper> Ptr_t;

public:
    clMainFrameHelper(clMainFrame* frame, clDockingManager* dockMgr);
    ~clMainFrameHelper();

    void SetDebuggerFeatures(size_t debuggerFeatures) { this->m_debuggerFeatures = debuggerFeatures; }
    size_t GetDebuggerFeatures() const { return m_debuggerFeatures; }

    /**
     * @brief is the toolbar visible?
     */
    bool IsToolbarShown() const;

    /**
     * @brief return true if the various docking windows captions are visible
     */
    bool IsCaptionsVisible() const;

    /**
     * @brief return true if the status bar is visible
     */
    bool IsStatusBarVisible() const;
};

#endif // CLMAINFRAMEHELPER_H
