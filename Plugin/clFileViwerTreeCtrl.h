#ifndef CLFILEVIWERTREECTRL_H
#define CLFILEVIWERTREECTRL_H

#include <wx/treectrl.h>
#include "codelite_exports.h"
#include <wx/filename.h>

// Item data class
class WXDLLIMPEXP_SDK clTreeCtrlData : public wxTreeItemData
{
public:
    enum eKind {
        kDummy = -1,
        kRoot,   // the hidden root folder
        kFile,   // a file
        kFolder, // a folder
    };
    eKind m_kind;
    // If file, contains the fullpath otherwise, contains the path
    wxString m_path;
    wxString m_name;

public:
    clTreeCtrlData(eKind kind)
        : m_kind(kind)
    {
    }
    virtual ~clTreeCtrlData() {}

    void SetKind(const eKind& kind) { this->m_kind = kind; }
    const eKind& GetKind() const { return m_kind; }
    
    /**
     * @brief set the path and update the display name
     */
    void SetPath(const wxString& path)
    {
        this->m_path = path;
        if(IsFolder()) {
            wxFileName fn(m_path);
            if(fn.GetDirCount()) {
                m_name = fn.GetDirs().Last();
            } else {
                m_name = m_path;
            }
        } else if(IsFile()){
            wxFileName fn(m_path);
            m_name = fn.GetFullName();
        } else {
            m_name = wxEmptyString;
        }
    }
    
    /**
     * @brief return the item's path. if it is a folder, return the folder path. If it is a file
     * return the file's fullpath
     */
    const wxString& GetPath() const { return m_path; }

    /**
     * @brief return the display name
     */
    const wxString& GetName() const { return m_name; }

    // Aliases
    bool IsFolder() const { return m_kind == kFolder; }
    bool IsFile() const { return m_kind == kFile; }
    bool IsDummy() const { return m_kind == kDummy; }
};

class WXDLLIMPEXP_SDK clFileViewerTreeCtrl : public wxTreeCtrl
{
    wxDECLARE_DYNAMIC_CLASS(clFileViewerTreeCtrl);

public:
    clFileViewerTreeCtrl();
    clFileViewerTreeCtrl(wxWindow* parent,
                        wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxTR_DEFAULT_STYLE | wxTR_MULTIPLE | wxTR_HIDE_ROOT | wxBORDER_STATIC);
    virtual ~clFileViewerTreeCtrl();

    virtual int OnCompareItems(const clTreeCtrlData* a, const clTreeCtrlData* b);
    virtual int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
};

#endif // CLFILEVIWERTREECTRL_H
