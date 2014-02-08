#ifndef DIFFSIDEBYSIDEPANEL_H
#define DIFFSIDEBYSIDEPANEL_H

#include "wxcrafter_plugin.h"
#include <wx/filename.h>
#include <vector>

class WXDLLIMPEXP_SDK DiffSideBySidePanel : public DiffSideBySidePanelBase
{
    typedef std::vector< int > Markers_t;

    Markers_t m_leftRedMarkers;             /// left view list of lines with red markers ("removed")
    Markers_t m_leftPlaceholdersMarkers;    /// left view list of lines with red markers ("removed")
    Markers_t m_rightGreenMarkers;          /// right view list of lines with green markers ("added")
    Markers_t m_rightPlaceholdersMarkers;   /// right view list of lines with green markers ("added")
    std::vector< std::pair<int, int> > m_sequences; // start-line - end-line pairs
    int m_cur_sequence;
    std::pair<int, int> m_selectedSequence;
    
protected:
    virtual void OnCopyLeftToRightUI(wxUpdateUIEvent& event);
    virtual void OnCopyRightToLeftUI(wxUpdateUIEvent& event);
    virtual void OnNextDiffUI(wxUpdateUIEvent& event);
    virtual void OnPrevDiffUI(wxUpdateUIEvent& event);
    virtual void OnNextDiffSequence(wxRibbonButtonBarEvent& event);
    virtual void OnPrevDiffSequence(wxRibbonButtonBarEvent& event);
    virtual void OnRefreshDiff(wxRibbonButtonBarEvent& event);
    virtual void OnLeftStcPainted(wxStyledTextEvent& event);
    virtual void OnRightStcPainted(wxStyledTextEvent& event);

    void PrepareViews();
    void UpdateViews(const wxString &left, const wxString &right);
    void DoClean();
    void DoDrawSequenceMarkers(int firstLine, int lastLine);

public:
    DiffSideBySidePanel(wxWindow* parent);
    virtual ~DiffSideBySidePanel();

    /**
     * @brief display a diff view for 2 files left and right
     */
    void Diff();

    /**
     * @brief set the initial files to diff
     * Once set, you should call Diff() function
     */
    void SetFiles(const wxFileName& left, const wxFileName &right);

    void SetLeftFileReadOnly(bool b);
    void SetRightFileReadOnly(bool b);
};
#endif // DIFFSIDEBYSIDEPANEL_H
