#ifndef NEWFILECOMPARISON_H
#define NEWFILECOMPARISON_H
#include "CodeLiteDiff_UI.h"
#include <wx/filename.h>

class NewFileComparison : public NewFileComparisonBase
{
public:
    NewFileComparison(wxWindow* parent, const wxFileName& leftFile);
    virtual ~NewFileComparison() = default;

protected:
    virtual void OnFileSelected(wxCommandEvent& event);
    virtual void OnBrowse(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // NEWFILECOMPARISON_H
