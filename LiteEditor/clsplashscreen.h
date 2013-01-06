#ifndef CLSPLASHSCREEN_H
#define CLSPLASHSCREEN_H

#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/bitmap.h>

class clSplashScreen : public wxFrame
{
    wxBitmap m_bmp;
    wxTimer  m_timer;
    
public:
    clSplashScreen(wxWindow* parent, const wxBitmap& bmp);
    virtual ~clSplashScreen();
    
    DECLARE_EVENT_TABLE()
    void OnPaint(wxPaintEvent& e);
    void OnEraseBg(wxEraseEvent& e);
    void OnTimer(wxTimerEvent &e);
};

#endif // CLSPLASHSCREEN_H
