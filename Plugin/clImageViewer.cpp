#include "clImageViewer.h"

#include <wx/dcbuffer.h>
#include <wx/settings.h>

clImageViewer::clImageViewer(wxWindow* parent, const wxFileName& filename)
    : clImageViewerBase(parent)
    , m_filename(filename)
{
    // Consruct a bitmap
    wxBitmap bmp;
    if(bmp.LoadFile(filename.GetFullPath(), wxBITMAP_TYPE_ANY)) {
        m_bitmap = bmp;
        m_staticBitmap->SetBitmap(m_bitmap);
    }
}

clImageViewer::~clImageViewer() {}