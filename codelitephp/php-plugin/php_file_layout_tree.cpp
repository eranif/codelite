#include "php_file_layout_tree.h"
#include <imanager.h>
#include <ieditor.h>
#include <algorithm>
#include <globals.h>

#ifndef __WXMSW__
#include <wx/imaglist.h>
#endif

PHPFileLayoutTree::PHPFileLayoutTree(wxWindow* parent, IEditor *editor, IManager *manager)
    : wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_FULL_ROW_HIGHLIGHT|wxTR_HIDE_ROOT|wxTR_NO_LINES|wxTR_SINGLE)
    , m_editor(editor)
    , m_manager(manager)
{
    MSWSetNativeTheme(this);
}

PHPFileLayoutTree::PHPFileLayoutTree(wxWindow* parent)
    : wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_FULL_ROW_HIGHLIGHT|wxTR_HIDE_ROOT|wxTR_NO_LINES|wxTR_SINGLE)
    , m_editor(NULL)
    , m_manager(NULL)
{
    MSWSetNativeTheme(this);
}

PHPFileLayoutTree::~PHPFileLayoutTree()
{
}

void PHPFileLayoutTree::Construct()
{
    // Sanity
    if(!m_editor || !m_manager)
        return;

    wxString text = m_editor->GetTextRange(0, m_editor->GetLength());
    if ( !PHPParser.parseString(true, text, PHPGlobals::Type_Normal, true, m_editor->GetFileName().GetFullPath()) ) {
        return;
    }

    DeleteAllItems();
    wxTreeItemId root = AddRoot(wxT("Root"));

    wxImageList *images = new wxImageList(16, 16, true);
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

    //////////////////////////////////////////////////////////////////////
    // Add the global variables
    //////////////////////////////////////////////////////////////////////

    std::map<wxString, PHPEntry> vars = PHPParser.getGlobalVariables();
    // Convert to vector and sort it alphabetic
    std::vector<PHPEntry*> vVars;
    std::map<wxString, PHPEntry>::iterator iter = vars.begin();
    for(; iter != vars.end(); iter++) {
        vVars.push_back(new PHPEntry(iter->second));
    }

    if(!vVars.empty()) {
        wxTreeItemId globalVarsId = AppendItem(root, wxT("Global Variables"), 0, 0);
        AddEntries(globalVarsId, vVars);
    }

    //////////////////////////////////////////////////////////////////////
    // Add the global constants
    //////////////////////////////////////////////////////////////////////

    std::map<wxString, PHPEntry> constants = PHPParser.getGlobalConstants();
    // Convert to vector and sort it alphabetic
    std::vector<PHPEntry*> vConstants;
    iter = constants.begin();
    for(; iter != constants.end(); iter++) {
        vConstants.push_back(new PHPEntry(iter->second));
    }

    if(!vConstants.empty()) {
        wxTreeItemId globalConstantsId = AppendItem(root, wxT("Global Constants"), 0, 0);
        AddEntries(globalConstantsId, vConstants);
    }

    //////////////////////////////////////////////////////////////////////
    // add the globals node
    //////////////////////////////////////////////////////////////////////

    std::map<wxString, PHPEntry> funcs = PHPParser.getGlobalFunctions();
    // Convert to vector and sort it alphabetic
    std::vector<PHPEntry*> vFuncs;
    iter = funcs.begin();
    for(; iter != funcs.end(); iter++) {
        vFuncs.push_back(new PHPEntry(iter->second));
    }

    if(!vFuncs.empty()) {
        wxTreeItemId globalFuncsId = AppendItem(root, wxT("Global Functions"), 0, 0);
        AddEntries(globalFuncsId, vFuncs);
    }

    // add the classes
    std::vector<PHPEntry*>& classes = PHPParser.getClasses();
    for(size_t i=0; i<classes.size(); i++) {
        wxTreeItemId classItem = AddClassItemToTreeWithNamespace(*classes.at(i));
        if(classItem.IsOk()) {
            std::vector<PHPEntry*> vEntries;
            PHPEntry *cls = classes.at(i);
            for(size_t j=0; j<cls->getEntries().size(); j++) {
                vEntries.push_back( new PHPEntry(cls->getEntries().at(j)) );
            }
            AddEntries(classItem, vEntries);
        }
    }

    if(HasChildren(GetRootItem())) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(GetRootItem(), cookie);
        if(child.IsOk()) {
            SelectItem(child);
            ScrollTo(child);
        }
        ExpandAll();
    }
}

int PHPFileLayoutTree::GetImageId(const PHPEntry* entry)
{
    if(entry->getKind() == PHP_Kind_Function) {
        if(entry->getAttributes() & PHP_Access_Private)
            return 1;
        else if(entry->getAttributes() & PHP_Access_Protected)
            return 2;
        else
            return 3;

    } else if(entry->getKind() == PHP_Kind_Variable) {
        return 6; // public

    } else if(entry->getKind() == PHP_Kind_Member) {
        if(entry->getAttributes() & PHP_Access_Private)
            return 4;
        else if(entry->getAttributes() & PHP_Access_Protected)
            return 5;
        else
            return 6;

    } else if(entry->getKind() == PHP_Kind_Constant) {
        return 9;

    }
    return -1; // Unknown
}

wxTreeItemId PHPFileLayoutTree::AddClassItemToTreeWithNamespace(const PHPEntry& cls)
{
    wxTreeItemId parent = GetRootItem();
    wxString ns = cls.getNameSpace();
    wxArrayString nameSpaces = wxStringTokenize(ns, wxT("\\"), wxTOKEN_STRTOK);
    for(size_t i=0; i<nameSpaces.GetCount(); i++) {
        parent = FindItemIdByName(parent, nameSpaces.Item(i));
        if(!parent.IsOk())
            return wxTreeItemId();
    }
    return AppendItem(parent, cls.getName(), 8, 8, new QItemData(new PHPEntry(cls)));
}

void PHPFileLayoutTree::AddEntries(wxTreeItemId& parent, std::vector<PHPEntry*>& entries)
{
    std::sort(entries.begin(), entries.end(), PHPEntrySorterP());
    for(size_t i=0; i<entries.size(); i++) {
        int imgId = GetImageId(entries.at(i));
        AppendItem(parent, entries[i]->getName(), imgId, imgId, new QItemData(entries.at(i)));
    }
}

wxTreeItemId PHPFileLayoutTree::FindItemIdByName(const wxTreeItemId &parent, const wxString& name)
{
    if(parent.IsOk() == false)
        return wxTreeItemId();

    if(!ItemHasChildren(parent)) {
        return AppendItem(parent, name, 7, 7, NULL);
    }

    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetFirstChild(parent, cookie);
    while ( child.IsOk() ) {
        if(GetItemText(child) == name) {
            return child;
        }
        child = GetNextChild(parent, cookie);
    }

    // No match? add it
    return AppendItem(parent, name, 7, 7, NULL);
}

wxTreeItemId PHPFileLayoutTree::RecurseSearch(const wxTreeItemId& item, const wxString& word)
{
    if(!item.IsOk())
        return wxTreeItemId();

    if(item != GetRootItem()) {
        wxString curtext = GetItemText(item);
        curtext.MakeLower();

        if(curtext.StartsWith(word)) {
            return item;
        }
    }

    if(ItemHasChildren(item)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(item, cookie);
        while( child.IsOk() ) {
            wxTreeItemId selection = RecurseSearch(child, word);
            if(selection.IsOk()) {
                return selection;
            }
            child = GetNextChild(item, cookie);
        }
    }
    return wxTreeItemId();
}

void PHPFileLayoutTree::FindWord(const wxString& word)
{
    wxString lcword = word;
    lcword.MakeLower();

    wxTreeItemId item = RecurseSearch(GetRootItem(), lcword);
    if(item.IsOk()) {
        SelectItem(item);
        EnsureVisible(item);
        ScrollTo(item);
    }
}

void PHPFileLayoutTree::AdvanceSelection(bool forward)
{
    wxTreeItemId item = GetSelection();
    if (!item.IsOk()) {
        return;
    }

    wxTreeItemId nextItem;
    if (forward) {
        //Item is visible, scroll to it to make sure GetNextVisible() wont
        //fail
        ScrollTo(item);
        nextItem = GetNextVisible(item);
    } else {
        nextItem = TryGetPrevItem(item);
    }

    if (nextItem.IsOk()) {
        SelectItem(nextItem);
    }
}

wxTreeItemId PHPFileLayoutTree::TryGetPrevItem(wxTreeItemId item)
{
    wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

    // find out the starting point
    wxTreeItemId prevItem = GetPrevSibling(item);
    if ( !prevItem.IsOk() ) {
        prevItem = GetItemParent(item);
        if ( prevItem == GetRootItem() ) {
            return wxTreeItemId();
        }
    }

    // from there we must be able to navigate until this item
    while ( prevItem.IsOk() ) {

        ScrollTo(prevItem);

        if(!IsVisible(prevItem)) {
            return wxTreeItemId();
        }

        const wxTreeItemId nextItem = GetNextVisible(prevItem);
        if ( !nextItem.IsOk() || nextItem == item )
            return prevItem;

        prevItem = nextItem;
    }

    return wxTreeItemId();
}
