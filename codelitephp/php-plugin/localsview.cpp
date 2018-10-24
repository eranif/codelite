#include "XDebugManager.h"
#include "editor_config.h"
#include "globals.h"
#include "localsview.h"
#include <editor_config.h>
#include <event_notifier.h>
#include <file_logger.h>
#include <lexer_configuration.h>

class MyStringData : public wxTreeItemData
{
    wxString m_data;

public:
    MyStringData(const wxString& data)
        : m_data(data)
    {
    }
    virtual ~MyStringData() {}
    void SetData(const wxString& data) { this->m_data = data; }
    const wxString& GetData() const { return m_data; }
};

LocalsView::LocalsView(wxWindow* parent)
    : LocalsViewBase(parent)
{
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_LOCALS_UPDATED, &LocalsView::OnLocalsUpdated, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_ENDED, &LocalsView::OnXDebugSessionEnded, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_STARTED, &LocalsView::OnXDebugSessionStarted, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_PROPERTY_GET, &LocalsView::OnProperytGet, this);
    
    ClearView();
    m_tree->AddHeader(_("Name"));
    m_tree->AddHeader(_("Type"));
    m_tree->AddHeader(_("Class"));
    m_tree->AddHeader(_("Value"));
    m_tree->Bind(wxEVT_TREE_ITEM_COLLAPSED, &LocalsView::OnLocalCollapsed, this);
    m_tree->Bind(wxEVT_TREE_ITEM_EXPANDING, &LocalsView::OnLocalExpanding, this);
    m_tree->Bind(wxEVT_TREE_ITEM_EXPANDED, &LocalsView::OnLocalExpanded, this);
    m_tree->Bind(wxEVT_TREE_ITEM_MENU, &LocalsView::OnLocalsMenu, this);
}

LocalsView::~LocalsView()
{
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_LOCALS_UPDATED, &LocalsView::OnLocalsUpdated, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_ENDED, &LocalsView::OnXDebugSessionEnded, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_STARTED, &LocalsView::OnXDebugSessionStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_PROPERTY_GET, &LocalsView::OnProperytGet, this);
}

void LocalsView::OnLocalCollapsed(wxTreeEvent& event)
{
    /// We keep track on expanded and collapsed items by their fullname
    CHECK_ITEM_RET(event.GetItem());
    MyStringData* cd = dynamic_cast<MyStringData*>(m_tree->GetItemData(event.GetItem()));
    CHECK_PTR_RET(cd);
    if(m_localsExpandedItemsFullname.count(cd->GetData())) { m_localsExpandedItemsFullname.erase(cd->GetData()); }
}

void LocalsView::OnXDebugSessionEnded(XDebugEvent& e)
{
    e.Skip();
    CL_DEBUG("LocalsView::OnXDebugSessionEnded(): Debug sessions started - cleaning all locals view");
    // Clear the variables view
    ClearView();
    m_localsExpandedItemsFullname.clear();
    m_localsExpandedItems.Clear();
    m_waitingExpand.clear();
}

void LocalsView::OnLocalExpanded(wxTreeEvent& event)
{
    /// We keep track on expanded and collapsed items by their fullname
    CHECK_ITEM_RET(event.GetItem());
    MyStringData* cd = dynamic_cast<MyStringData*>(m_tree->GetItemData(event.GetItem()));
    CHECK_PTR_RET(cd);
    m_localsExpandedItemsFullname.insert(cd->GetData());
}

void LocalsView::OnLocalsUpdated(XDebugEvent& e)
{
    e.Skip();
    CL_DEBUG("Inside OnLocalsUpdated");

    ClearView();
    m_localsExpandedItems.Clear();

    const XVariable::List_t& vars = e.GetVariables();
    AppendVariablesToTree(m_tree->GetRootItem(), vars);

    // Expand the items that were expanded before the view refresh
    for(size_t i = 0; i < m_localsExpandedItems.GetCount(); ++i) {
        // Ensure it is visible
        m_tree->EnsureVisible(m_localsExpandedItems.Item(i));
        // Ensure its expanded
        m_tree->Expand(m_localsExpandedItems.Item(i));
    }
    m_localsExpandedItems.Clear();
}

void LocalsView::AppendVariablesToTree(const wxTreeItemId& parent, const XVariable::List_t& children)
{
    XVariable::List_t::const_iterator iter = children.begin();
    for(; iter != children.end(); ++iter) {
        const XVariable& var = *iter;
        wxTreeItemId item =
            m_tree->AppendItem(parent, var.name, wxNOT_FOUND, wxNOT_FOUND, new MyStringData(var.fullname));
        m_tree->SetItemText(item, var.type, 1);
        m_tree->SetItemText(item, var.classname, 2);
        m_tree->SetItemText(item, var.value, 3);

        if(var.GetCreateFakeNode()) {
            // create dummy node in the tree view so we can expand it later
            m_tree->AppendItem(item, "<dummy>");

        } else if(var.HasChildren()) {
            AppendVariablesToTree(item, var.children);
            if(m_localsExpandedItemsFullname.count(var.fullname)) {
                // this item should be expanded
                m_localsExpandedItems.Add(item);
            }
        }
    }
}

void LocalsView::OnXDebugSessionStarted(XDebugEvent& e)
{
    e.Skip();
    CL_DEBUG("LocalsView::OnXDebugSessionStarted(): Debug sessions started - cleaning all locals view");
    // Clear the variables view
    ClearView();
    m_localsExpandedItemsFullname.clear();
    m_localsExpandedItems.Clear();
    m_waitingExpand.clear();
}

void LocalsView::OnLocalExpanding(wxTreeEvent& event)
{
    event.Skip();
    CHECK_ITEM_RET(event.GetItem());

    wxTreeItemId item = event.GetItem();
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_tree->GetFirstChild(item, cookie);
    if(child.IsOk() && (m_tree->GetItemText(child) == "<dummy>")) {

        // a dummy node has been found
        // Delete this node and request from XDebug to expand it
        m_tree->SetItemText(child, wxString("Loading..."));
        wxString propertyName = DoGetItemClientData(event.GetItem());
        XDebugManager::Get().SendGetProperty(propertyName);
        m_waitingExpand.insert({ propertyName, item });
    }
}

wxString LocalsView::DoGetItemClientData(const wxTreeItemId& item) const
{
    MyStringData* scd = dynamic_cast<MyStringData*>(m_tree->GetItemData(item));
    if(scd) { return scd->GetData(); }
    return wxEmptyString;
}

void LocalsView::OnProperytGet(XDebugEvent& e)
{
    e.Skip();
    // An item was evaluated using property_get
    std::unordered_map<wxString, wxTreeItemId>::iterator iter = m_waitingExpand.find(e.GetEvaluted());
    if(iter == m_waitingExpand.end()) { return; }

    wxTreeItemId item = iter->second;
    m_waitingExpand.erase(iter);

    // Delete the fake node
    m_tree->DeleteChildren(item);

    XVariable::List_t vars = e.GetVariables();
    if(vars.empty()) return;

    // Since we got here from property_get, XDebug will reply with the specific property (e.g. $myclass->secondClass)
    // and all its children. Howeverr, $myclass->secondClass already exist in the tree
    // so we are only interested with its children. so we use here vars.begin()->children (vars is always list of size
    // == 1)
    wxASSERT_MSG(vars.size() == 1, "property_get returned list of size != 1");
    XVariable::List_t childs;
    childs = vars.begin()->children;

    if(!childs.empty()) {
        AppendVariablesToTree(item, childs);
        m_tree->Expand(item);
    }
}

void LocalsView::OnLocalsMenu(wxTreeEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("php_locals_copy_value"), _("Copy Value"));

    menu.Bind(wxEVT_MENU, &LocalsView::OnCopyValue, this, XRCID("php_locals_copy_value"));
    m_tree->PopupMenu(&menu);
}

void LocalsView::OnCopyValue(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    m_tree->GetSelections(items);

    wxString textToCopy;
    for(size_t i = 0; i < items.size(); ++i) {
        wxString v = m_tree->GetItemText(items.Item(i), 3);
        textToCopy << v << EditorConfigST::Get()->GetOptions()->GetEOLAsString();
    }
    ::CopyToClipboard(textToCopy);
}

void LocalsView::ClearView()
{
    m_tree->DeleteAllItems();
    wxTreeItemId root = m_tree->AddRoot(_("Locals"), wxNOT_FOUND, wxNOT_FOUND, new MyStringData(""));
    m_tree->Expand(root);
}
