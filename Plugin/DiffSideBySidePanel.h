#ifndef DIFFSIDEBYSIDEPANEL_H
#define DIFFSIDEBYSIDEPANEL_H

#include "wxcrafter_plugin.h"
#include <wx/filename.h>
#include <vector>

class WXDLLIMPEXP_SDK DiffSideBySidePanel : public DiffSideBySidePanelBase
{
    typedef std::vector< int > Markers_t;
public:
    struct FileInfo {
        wxFileName filename;
        wxString title;
        bool readOnly;
        bool deleteFileOnDestroy;

        FileInfo(const wxFileName& fn, const wxString &caption, bool ro) : filename(fn), title(caption), readOnly(ro), deleteFileOnDestroy(false) {}
        FileInfo() : readOnly(true), deleteFileOnDestroy(false) {}

        void Clear() {
            filename.Clear();
            title.Clear();
            readOnly = true;
            deleteFileOnDestroy = false;
        }

        void DeleteFileIfNeeded() {
            if ( deleteFileOnDestroy && filename.IsOk() && filename.Exists() ) {
                ::wxRemoveFile( filename.GetFullPath() );
            }
            Clear();
        }
    };

    Markers_t m_leftRedMarkers;             /// left view list of lines with red markers ("removed")
    Markers_t m_leftPlaceholdersMarkers;    /// left view list of lines with red markers ("removed")
    Markers_t m_rightGreenMarkers;          /// right view list of lines with green markers ("added")
    Markers_t m_rightPlaceholdersMarkers;   /// right view list of lines with green markers ("added")
    std::vector< std::pair<int, int> > m_sequences; // start-line - end-line pairs
    int m_cur_sequence;

    DiffSideBySidePanel::FileInfo m_leftFile;
    DiffSideBySidePanel::FileInfo m_rightFile;

protected:
    virtual void OnSaveChanges(wxRibbonButtonBarEvent& event);
    virtual void OnSaveChangesUI(wxUpdateUIEvent& event);
    virtual void OnCopyLeftToRight(wxRibbonButtonBarEvent& event);
    virtual void OnCopyRightToLeft(wxRibbonButtonBarEvent& event);
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
    void DoDrawSequenceMarkers(int firstLine, int lastLine, wxStyledTextCtrl* ctrl);
    void DoCopyCurrentSequence(wxStyledTextCtrl* from, wxStyledTextCtrl* to);
    void DoGetPositionsToCopy(wxStyledTextCtrl* stc, int& startPos, int& endPos, int& placeHolderMarkerFirstLine, int& placeHolderMarkerLastLine);
    void DoSave(wxStyledTextCtrl* stc, const wxFileName& fn);
    
    bool CanNextDiff();
    bool CanPrevDiff();
    
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
    void SetFilesDetails(const DiffSideBySidePanel::FileInfo& leftFile, const DiffSideBySidePanel::FileInfo& rightFile);
};
#endif // DIFFSIDEBYSIDEPANEL_H
