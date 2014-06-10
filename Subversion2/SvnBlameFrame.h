#ifndef SVNBLAMEFRAME_H
#define SVNBLAMEFRAME_H

#include "subversion2_ui.h"
#include <wx/filename.h>

class SvnBlameFrame : public SvnBlameFrameBase
{
    wxFileName m_filename;

public:
    SvnBlameFrame(wxWindow* parent, const wxFileName& filename, const wxString &content);
    virtual ~SvnBlameFrame();
};
#endif // SVNBLAMEFRAME_H
