#ifndef CLDIFFFRAME_H
#define CLDIFFFRAME_H

#include "DiffSideBySidePanel.h"
#include "codelite_exports.h"
#include <wx/frame.h>

class WXDLLIMPEXP_SDK clDiffFrame : public wxDialog
{
public:
    clDiffFrame(wxWindow* parent, const DiffSideBySidePanel::FileInfo& left, const DiffSideBySidePanel::FileInfo& right,
                bool originSourceControl);
    clDiffFrame(wxWindow* parent);
    clDiffFrame(wxWindow* parent, const wxFileName& left, const wxFileName& right, bool isTempFile);
    virtual ~clDiffFrame();
};

#endif // CLDIFFFRAME_H
