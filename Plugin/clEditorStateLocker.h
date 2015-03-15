#ifndef CLEDITORSTATELOCKER_H
#define CLEDITORSTATELOCKER_H

#include "codelite_exports.h"
#include <wx/stc/stc.h>
#include <wx/arrstr.h>
#include <vector>

class WXDLLIMPEXP_SDK clEditorStateLocker
{
public:
    typedef std::vector<int> VecInt_t;

private:
    // The editor
    wxStyledTextCtrl* m_ctrl;

    // States:
    // Visible line
    int m_firstVisibleLine;

    // Bookrmarks
    wxArrayString m_bookmarks;
    
    // Breakpoints
    wxArrayString m_breakpoints;
    
    // Folds
    clEditorStateLocker::VecInt_t m_folds;
    
    // Caret position
    int m_position;
    
private:
    void SerializeBookmarks();
    void SerializeFolds();
    void SerializeBreakpoints();
    
    void ApplyBookmarks();
    void ApplyFolds();
    void ApplyBreakpoints();

public:
    clEditorStateLocker(wxStyledTextCtrl* ctrl);
    virtual ~clEditorStateLocker();

    /**
     * @brief serialize the editor bookmarks into an array
     * @param bookmarks [output]
     */
    static void SerializeBookmarks(wxStyledTextCtrl* ctrl, wxArrayString& bookmarks);
    /**
     * @brief apply bookmarks to the editor (serialized by SerializeBookmarks earlier)
     * @param bookmarks serialized bookmarks
     */
    static void ApplyBookmarks(wxStyledTextCtrl* ctrl, const wxArrayString& bookmarks);

    /**
     * Apply collapsed folds from a vector
     */
    static void ApplyFolds(wxStyledTextCtrl* ctrl, const clEditorStateLocker::VecInt_t& folds);

    /**
     * Store any collapsed folds to a vector, so they can be serialised
     */
    static void SerializeFolds(wxStyledTextCtrl* ctrl, clEditorStateLocker::VecInt_t& folds);
    
    /**
     * @brief serialize Breakpoints markers into an array
     */
    static void SerializeBreakpoints(wxStyledTextCtrl* ctrl, wxArrayString &breapoints);
    
    /**
     * @brief apply breapoints markers
     */
    static void ApplyBreakpoints(wxStyledTextCtrl* ctrl, const wxArrayString &breapoints);
};

#endif // CLEDITORSTATELOCKER_H
