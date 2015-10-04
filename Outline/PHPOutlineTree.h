#ifndef PHPOUTLINETREE_H
#define PHPOUTLINETREE_H

#include <wx/treectrl.h>
#include <wx/filename.h>
#include "PHPEntityBase.h"
#include "imanager.h"

class PHPOutlineTree : public wxTreeCtrl
{
    wxFileName m_filename;
    IManager* m_manager;

protected:
    void BuildTree(wxTreeItemId parentTreeItem, PHPEntityBase::Ptr_t entity);
    int GetImageId(PHPEntityBase::Ptr_t entry);
    void SetEditorActive(IEditor* editor);
    wxTreeItemId DoFind(const wxString& pattern, const wxTreeItemId& parent);

public:
    PHPOutlineTree(wxWindow* parent,
                   wxWindowID id = wxID_ANY,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0);
    virtual ~PHPOutlineTree();
    void BuildTree(const wxFileName& filename);
    void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
    const wxFileName& GetFilename() const { return m_filename; }
    void Clear();
    void SetManager(IManager* manager) { this->m_manager = manager; }
    IManager* GetManager() { return m_manager; }
    void ItemSelected(const wxTreeItemId& item, bool focusEditor);
    bool Select(const wxString& pattern);
};

#endif // PHPOUTLINETREE_H
