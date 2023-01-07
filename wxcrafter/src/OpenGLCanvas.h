#ifndef OPENGLCANVAS_H
#define OPENGLCANVAS_H
#include "wxcrafter_gui.h"

class OpenGLCanvas : public OpenGLCanvasBase
{
public:
    OpenGLCanvas(wxWindow* parent);
    virtual ~OpenGLCanvas();
    wxStaticBitmap* Bitmap() const { return m_staticBitmap657; }

protected:
    virtual void OnMove(wxMoveEvent& event);
    virtual void OnSize(wxSizeEvent& event);
};
#endif // OPENGLCANVAS_H
