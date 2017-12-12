#include "PHPEntityFunction.h"
#include "PHPEntityVariable.h"
#include "PHPOutlineTree.h"
#include "PHPSourceFile.h"
#include "drawingutils.h"
#include "fileutils.h"
#include "globals.h"
#include "navigationmanager.h"
#include <algorithm>
#include <globals.h>
#include <ieditor.h>
#include <imanager.h>
#include <wx/wupdlock.h>

#ifndef __WXMSW__
#include <wx/imaglist.h>
#endif

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

PHPOutlineTree::PHPOutlineTree(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxTreeCtrl(parent, id, pos, size, style)
{
    MSWSetNativeTheme(this);
    SetFont(DrawingUtils::GetDefaultGuiFont());
}

PHPOutlineTree::~PHPOutlineTree() {}

int PHPOutlineTree::GetImageId(PHPEntityBase::Ptr_t entry)
{
    if(entry->Is(kEntityTypeFunction)) {
        PHPEntityFunction* func = entry->Cast<PHPEntityFunction>();

        if(func->HasFlag(kFunc_Private))
            return 1;
        else if(func->HasFlag(kFunc_Protected))
            return 2;
        else
            // public
            return 3;

    } else if(entry->Is(kEntityTypeVariable)) {
        PHPEntityVariable* var = entry->Cast<PHPEntityVariable>();
        if(!var->IsMember() && !var->IsConst()) {
            // A global variale
            return 6;

        } else if(var->IsMember()) {
            if(var->HasFlag(kVar_Const)) return 9; // constant
            // Member
            if(var->HasFlag(kVar_Private))
                return 4;
            else if(var->HasFlag(kVar_Protected))
                return 5;
            else
                return 6;

        } else if(var->IsConst()) {
            // Constant
            return 9;
        } else {
            return 6;
        }

    } else if(entry->Is(kEntityTypeNamespace)) {
        // Namespace
        return 7;
    } else if(entry->Is(kEntityTypeClass)) {
        return 8;
    }
    return -1; // Unknown
}

void PHPOutlineTree::BuildTree(const wxFileName& filename)
{
    m_filename = filename;
    PHPSourceFile sourceFile(filename, NULL);
    sourceFile.SetParseFunctionBody(false);
    sourceFile.Parse();
    wxWindowUpdateLocker locker(this);
    DeleteAllItems();

    wxTreeItemId root = AddRoot(wxT("Root"));

    wxImageList* images = new wxImageList(clGetScaledSize(16), clGetScaledSize(16), true);
    images->Add(m_manager->GetStdIcons()->LoadBitmap(wxT("cc/16/globals")));            // 0
    images->Add(m_manager->GetStdIcons()->LoadBitmap(wxT("cc/16/function_private")));   // 1
    images->Add(m_manager->GetStdIcons()->LoadBitmap(wxT("cc/16/function_protected"))); // 2
    images->Add(m_manager->GetStdIcons()->LoadBitmap(wxT("cc/16/function_public")));    // 3
    images->Add(m_manager->GetStdIcons()->LoadBitmap(wxT("cc/16/member_private")));     // 4
    images->Add(m_manager->GetStdIcons()->LoadBitmap(wxT("cc/16/member_protected")));   // 5
    images->Add(m_manager->GetStdIcons()->LoadBitmap(wxT("cc/16/member_public")));      // 6
    images->Add(m_manager->GetStdIcons()->LoadBitmap(wxT("cc/16/namespace")));          // 7
    images->Add(m_manager->GetStdIcons()->LoadBitmap(wxT("cc/16/class")));              // 8
    images->Add(m_manager->GetStdIcons()->LoadBitmap(wxT("cc/16/enumerator")));         // 9
    AssignImageList(images);

    // Build the tree view
    BuildTree(root, sourceFile.Namespace());

    if(HasChildren(GetRootItem())) {
        ExpandAll();
    }
}

void PHPOutlineTree::Clear()
{
    wxWindowUpdateLocker locker(this);
    DeleteAllItems();
    m_filename.Clear();
}

void PHPOutlineTree::BuildTree(wxTreeItemId parentTreeItem, PHPEntityBase::Ptr_t entity)
{
    int imgID = GetImageId(entity);
    wxTreeItemId parent = AppendItem(parentTreeItem, entity->GetDisplayName(), imgID, imgID, new QItemData(entity));
    // dont add the children of the function (i.e. function arguments)
    if(entity->Is(kEntityTypeFunction)) return;
    const PHPEntityBase::List_t& children = entity->GetChildren();
    if(!children.empty()) {
        PHPEntityBase::List_t::const_iterator iter = children.begin();
        for(; iter != children.end(); ++iter) {
            BuildTree(parent, *iter);
        }
    }
}

void PHPOutlineTree::ItemSelected(const wxTreeItemId& item, bool focusEditor)
{
    QItemData* itemData = dynamic_cast<QItemData*>(GetItemData(item));
    CHECK_PTR_RET(itemData);

    IEditor* editor = m_manager->GetActiveEditor();
    CHECK_PTR_RET(editor);

    // Define the pattern to search

    editor->FindAndSelect(itemData->m_entry->GetShortName(), itemData->m_entry->GetShortName(),
                          editor->PosFromLine(itemData->m_entry->GetLine()), NavMgr::Get());
    // set the focus to the editor
    if(focusEditor) {
        CallAfter(&PHPOutlineTree::SetEditorActive, editor);
    }
}

void PHPOutlineTree::SetEditorActive(IEditor* editor) { editor->SetActive(); }

bool PHPOutlineTree::Select(const wxString& pattern)
{
    wxTreeItemId item = DoFind(pattern, GetRootItem());
    if(item.IsOk()) {
        // select this item
        EnsureVisible(item);
        SelectItem(item);
        return true;
    }
    return false;
}

wxTreeItemId PHPOutlineTree::DoFind(const wxString& pattern, const wxTreeItemId& parent)
{
    if((GetRootItem() != parent) && FileUtils::FuzzyMatch(pattern, GetItemText(parent))) {
        return parent;
    }
    if(ItemHasChildren(parent)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(parent, cookie);
        while(child.IsOk()) {
            wxTreeItemId match = DoFind(pattern, child);
            if(match.IsOk()) {
                return match;
            }
            child = GetNextChild(parent, cookie);
        }
    }
    return wxTreeItemId();
}
