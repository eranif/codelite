#include "OpenGLCanvas.h"

OpenGLCanvas::OpenGLCanvas(wxWindow* parent)
    : OpenGLCanvasBase(parent)
{
}

void OpenGLCanvas::OnMove(wxMoveEvent& event) { event.Skip(); }
void OpenGLCanvas::OnSize(wxSizeEvent& event) { event.Skip(); }
