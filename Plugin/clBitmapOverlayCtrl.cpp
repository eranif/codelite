#include "clBitmapOverlayCtrl.h"

#include "bitmap_loader.h"
#include "globals.h"
#include "imanager.h"

clBitmapOverlayCtrl::clBitmapOverlayCtrl(wxWindow* win, const wxString& bitmapname, size_t flags)
    : m_win(win)
    , m_name(bitmapname)
    , m_flags(flags)
    , m_bmpWindow(NULL)
{
    BitmapLoader* bmpLoader = clGetManager()->GetStdIcons();
    m_bmp = bmpLoader->LoadBitmap(bitmapname);
    DoPosition();
    m_win->Bind(wxEVT_SIZE, &clBitmapOverlayCtrl::OnSize, this);
}

clBitmapOverlayCtrl::~clBitmapOverlayCtrl()
{
    if(m_bmpWindow) {
        m_win->RemoveChild(m_bmpWindow);
        m_bmpWindow->Destroy();
    }
    m_win->Unbind(wxEVT_SIZE, &clBitmapOverlayCtrl::OnSize, this);
}

void clBitmapOverlayCtrl::DoPosition()
{
    if(m_bmp.IsOk() && m_win) {
        wxRect rect = m_win->GetRect();
        int xx = 0;
        int yy = 0;
        int spacer = 5;
        if(m_flags & wxRIGHT) {
            xx = rect.GetWidth() - m_bmp.GetScaledWidth() - spacer;
        } else {
            xx = spacer;
        }

        if(m_flags & wxCENTER) {
            yy = (rect.GetHeight() - m_bmp.GetScaledHeight()) / 2;
        } else if(m_flags & wxBOTTOM) {
            yy = (rect.GetHeight() - m_bmp.GetScaledHeight());
        } else {
            // Top
            yy = 0;
        }
        if(!m_bmpWindow) {
            m_bmpWindow = new wxStaticBitmap(m_win, wxID_ANY, m_bmp, wxPoint(xx, yy));
        }
        m_bmpWindow->Move(wxPoint(xx, yy));
    }
}

void clBitmapOverlayCtrl::OnSize(wxSizeEvent& event)
{
    event.Skip();
    DoPosition();
}
