#ifndef CLMOUSECAPTURELOCKER_H
#define CLMOUSECAPTURELOCKER_H

#include "codelite_exports.h"
#include <wx/window.h>

class WXDLLIMPEXP_SDK clMouseCaptureLocker
{
    wxWindow* m_win;

public:
    clMouseCaptureLocker();
    virtual ~clMouseCaptureLocker();

    /**
     * @brief capture the mouse (if not captured)
     * If this class already captured some other window mouse, release it before
     * capturing the mouse for the new window. If passing NULL, release any window captured by
     * this class and do nothing
     */
    void CaptureMouse(wxWindow* win);
};

#endif // CLMOUSECAPTURELOCKER_H
