#ifndef PHPFILELAYOUTTREE_H
#define PHPFILELAYOUTTREE_H

#include "precompiled_header.h"
#include "PHPEntityBase.h"

class IEditor;
class IManager;

class QItemData : public wxTreeItemData
{
public:
    PHPEntityBase::Ptr_t m_entry;

public:
    QItemData(PHPEntityBase::Ptr_t entry)
        : m_entry(entry)
    {
    }
    virtual ~QItemData() {}
};

class PHPFileLayoutTree : public wxTreeCtrl
{
protected:
    IEditor* m_editor;
    IManager* m_manager;

protected:
    void BuildTree(wxTreeItemId parentTreeItem, PHPEntityBase::Ptr_t entity);
    int GetImageId(PHPEntityBase::Ptr_t entry);
    wxTreeItemId FindItemIdByName(const wxTreeItemId& parent, const wxString& name);
    wxTreeItemId RecurseSearch(const wxTreeItemId& item, const wxString& word);
    wxTreeItemId TryGetPrevItem(wxTreeItemId item);

public:
    PHPFileLayoutTree(wxWindow* parent, IEditor* editor, IManager* manager);
    PHPFileLayoutTree(wxWindow* parent);
    virtual ~PHPFileLayoutTree();

    void Construct();
    void AdvanceSelection(bool forward);
    void SetEditor(IEditor* editor) { this->m_editor = editor; }
    void SetManager(IManager* manager) { this->m_manager = manager; }
    IEditor* GetEditor() { return m_editor; }
    IManager* GetManager() { return m_manager; }

    void FindWord(const wxString& word);
};

#endif // PHPFILELAYOUTTREE_H
