#ifndef CLTREECTRLCOLOURHELPER_H
#define CLTREECTRLCOLOURHELPER_H

#include "VirtualDirectoryColour.h"
#include "clThemedTreeCtrl.h"
#include "smart_ptr.h"
#include <codelite_exports.h>
#include <wx/colour.h>
#include <wx/treectrl.h>

class WXDLLIMPEXP_SDK clTreeCtrlColourHelper
{
    clThemedTreeCtrl* m_tree;

public:
    typedef SmartPtr<clTreeCtrlColourHelper> Ptr_t;

protected:
    wxString GetItemPath(const wxTreeItemId& item) const;

    void DoSetBgColour(const wxTreeItemId& item, const wxColour& currentBgColour,
                       const FolderColour::Map_t& coloursMap) const;

    void DoClearBgColour(const wxTreeItemId& item, const wxColour& colourToSet, FolderColour::Map_t& coloursMap) const;

public:
    clTreeCtrlColourHelper(clThemedTreeCtrl* tree);
    virtual ~clTreeCtrlColourHelper();

    void SetBgColour(const wxTreeItemId& item, const wxColour& currentBgColour, FolderColour::Map_t& coloursMap) const;

    void ResetBgColour(const wxTreeItemId& item, FolderColour::Map_t& coloursMap) const;

    /**
     * @brief return a text path representing a tree item
     */
    static wxString GetItemPath(clThemedTreeCtrl* tree, const wxTreeItemId& item);
};

#endif // CLTREECTRLCOLOURHELPER_H
