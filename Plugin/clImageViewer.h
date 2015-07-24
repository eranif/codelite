#ifndef CLIMAGEVIEWER_H
#define CLIMAGEVIEWER_H
#include "wxcrafter_plugin.h"
#include <wx/filename.h>

class WXDLLIMPEXP_SDK clImageViewer : public clImageViewerBase
{
    wxFileName m_filename;
    wxBitmap m_bitmap;
    
public:
    clImageViewer(wxWindow* parent, const wxFileName& filename);
    virtual ~clImageViewer();
    void DoLayout();
};

#endif // CLIMAGEVIEWER_H
