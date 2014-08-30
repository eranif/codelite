#ifndef PHPFILELAYOUTTREE_H
#define PHPFILELAYOUTTREE_H

#include "precompiled_header.h"
#include "php_parser_api.h"

class IEditor;
class IManager;

class QItemData : public wxTreeItemData
{
public:
    PHPEntry *m_entry;

public:
    QItemData(PHPEntry *phpEntry) : m_entry(phpEntry) {}
    virtual ~QItemData() {
        if(m_entry) delete m_entry;
        m_entry = NULL;
    }
};

class PHPFileLayoutTree : public wxTreeCtrl
{
protected:
    IEditor  *m_editor;
    IManager *m_manager;

protected:
    int  GetImageId(const PHPEntry* entry);
    void AddEntries(wxTreeItemId &parent, std::vector<PHPEntry*>& entries);
    wxTreeItemId AddClassItemToTreeWithNamespace(const PHPEntry &cls);
    wxTreeItemId FindItemIdByName(const wxTreeItemId &parent, const wxString &name);
    wxTreeItemId RecurseSearch(const wxTreeItemId& item, const wxString &word);
    wxTreeItemId TryGetPrevItem(wxTreeItemId item);

public:
    PHPFileLayoutTree(wxWindow* parent, IEditor *editor, IManager *manager);
    PHPFileLayoutTree(wxWindow* parent);
    virtual ~PHPFileLayoutTree();

    void Construct();
    void AdvanceSelection(bool forward);
    void SetEditor(IEditor* editor) {
        this->m_editor = editor;
    }
    void SetManager(IManager* manager) {
        this->m_manager = manager;
    }
    IEditor* GetEditor() {
        return m_editor;
    }
    IManager* GetManager() {
        return m_manager;
    }

    void FindWord(const wxString &word);
};

#endif // PHPFILELAYOUTTREE_H
