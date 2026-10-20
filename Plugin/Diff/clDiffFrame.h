#ifndef CLDIFFFRAME_H
#define CLDIFFFRAME_H

#include "DiffSideBySidePanel.h"
#include "codelite_exports.h"

#include <wx/panel.h>

/// A diff view that lives in a tab of the main notebook.
class WXDLLIMPEXP_SDK clDiffFrame : public wxPanel
{
protected:
    DiffSideBySidePanel* m_diffView = nullptr;

protected:
    clDiffFrame(wxWindow* parent,
                const DiffSideBySidePanel::FileInfo& left,
                const DiffSideBySidePanel::FileInfo& right,
                bool originSourceControl);
    explicit clDiffFrame(wxWindow* parent);
    clDiffFrame(wxWindow* parent, const wxFileName& left, const wxFileName& right, bool isTempFile);

    /// Add this panel to the main notebook, using the diff icon, and select it
    void ShowInTab(const wxString& title);

public:
    ~clDiffFrame() override = default;

    /// Open a diff of 2 files that have a title and are optionally read-only
    static clDiffFrame* Open(const DiffSideBySidePanel::FileInfo& left,
                             const DiffSideBySidePanel::FileInfo& right,
                             bool originSourceControl);

    /// Open an empty diff view, the user selects the files
    static clDiffFrame* OpenNew();

    /// Open a diff of 2 files
    static clDiffFrame* Open(const wxFileName& left, const wxFileName& right, bool isTempFile);
};

#endif // CLDIFFFRAME_H
