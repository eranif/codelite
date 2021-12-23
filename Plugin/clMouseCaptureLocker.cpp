#include "clMouseCaptureLocker.h"

clMouseCaptureLocker::clMouseCaptureLocker()
    : m_win(NULL)
{
}

clMouseCaptureLocker::~clMouseCaptureLocker()
{
    if(m_win) {
        m_win->ReleaseMouse();
    }
}

void clMouseCaptureLocker::CaptureMouse(wxWindow* win)
{
    if(m_win) {
        m_win->ReleaseMouse();
    }

    m_win = win;
    if(m_win && !m_win->HasCapture()) {
        m_win->CaptureMouse();
    }
}
