#ifndef OPENGLCANVAS_H
#define OPENGLCANVAS_H
#include "wxcrafter_gui.h"

class OpenGLCanvas : public OpenGLCanvasBase
{
public:
    OpenGLCanvas(wxWindow* parent);
    ~OpenGLCanvas() override = default;
    wxStaticBitmap* Bitmap() const { return m_staticBitmap657; }

protected:
    void OnMove(wxMoveEvent& event) override;
    void OnSize(wxSizeEvent& event) override;
};
#endif // OPENGLCANVAS_H
