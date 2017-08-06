#ifndef CLTREECTRLCOLOURHELPER_H
#define CLTREECTRLCOLOURHELPER_H

#include <codelite_exports.h>
#include <wx/treectrl.h>
#include <wx/colour.h>
#include "VirtualDirectoryColour.h"
#include "smart_ptr.h"

class WXDLLIMPEXP_SDK clTreeCtrlColourHelper
{
    wxTreeCtrl* m_tree;

public:
    typedef SmartPtr<clTreeCtrlColourHelper> Ptr_t;

protected:
    wxString GetItemPath(const wxTreeItemId& item) const;
    
    void DoSetBgColour(const wxTreeItemId& item, const wxColour& currentBgColour,
                       const FolderColour::Map_t& coloursMap) const;
                       
    void DoClearBgColour(const wxTreeItemId& item, const wxColour& colourToSet,
                         FolderColour::Map_t& coloursMap) const;
public:
    clTreeCtrlColourHelper(wxTreeCtrl* tree);
    virtual ~clTreeCtrlColourHelper();

    void SetBgColour(const wxTreeItemId& item, const wxColour& currentBgColour,
                     FolderColour::Map_t& coloursMap) const;

    void ResetBgColour(const wxTreeItemId& item, FolderColour::Map_t& coloursMap) const;
    
    /**
     * @brief return a text path representing a tree item
     */
    static wxString GetItemPath(wxTreeCtrl* tree, const wxTreeItemId& item);
};

#endif // CLTREECTRLCOLOURHELPER_H
