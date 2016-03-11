#ifndef CLMAINFRAMEHELPER_H
#define CLMAINFRAMEHELPER_H

#include "codelite_exports.h"
#include <wx/event.h>
#include "smart_ptr.h"

class clMainFrame;
class clDockingManager;

class WXDLLIMPEXP_SDK clMainFrameHelper : public wxEvtHandler
{
    clMainFrame* m_mainFrame;
    clDockingManager* m_mgr;

public:
    typedef SmartPtr<clMainFrameHelper> Ptr_t;

public:
    clMainFrameHelper(clMainFrame* frame, clDockingManager* dockMgr);
    ~clMainFrameHelper();

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
