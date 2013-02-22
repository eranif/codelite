#ifndef FINDUSAGETAB_H
#define FINDUSAGETAB_H

#include "outputtabwindow.h" // Base class OutputTabWindow
#include "cpptoken.h"

typedef std::map<int, CppToken> UsageResultsMap;

class FindUsageTab : public OutputTabWindow
{
    UsageResultsMap m_matches;
protected:
    void DoOpenResult(const CppToken& token);

public:
    FindUsageTab(wxWindow* parent, const wxString &name);
    virtual ~FindUsageTab();

public:
    virtual void Clear();
    virtual void OnClearAllUI(wxUpdateUIEvent& e);
    virtual void OnClearAll(wxCommandEvent& e);
    virtual void OnMouseDClick(wxStyledTextEvent& e);
    virtual void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
    virtual void OnStyleNeeded(wxStyledTextEvent& e);
    virtual void OnThemeChanged(wxCommandEvent &e);
public:
    void ShowUsage(const std::list<CppToken> &matches, const wxString &searchWhat);

};

#endif // FINDUSAGETAB_H
