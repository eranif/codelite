#include "tree_book_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include <wx/treebook.h>

TreeBookWrapper::TreeBookWrapper()
    : NotebookBaseWrapper(ID_WXTREEBOOK)
{
    SetPropertyString(_("Common Settings"), "wxTreebook");
    RegisterEvent(wxT("wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED"), wxT("wxTreebookEvent"),
                  _("The page selection was changed"));
    RegisterEvent(wxT("wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING"), wxT("wxTreebookEvent"),
                  _("The page selection is about to be changed. This event can be vetoed"));
    RegisterEvent(wxT("wxEVT_COMMAND_TREEBOOK_NODE_COLLAPSED"), wxT("wxTreebookEvent"),
                  _("The page node is going to be collapsed"));
    RegisterEvent(wxT("wxEVT_COMMAND_TREEBOOK_NODE_EXPANDED"), wxT("wxTreebookEvent"),
                  _("The page node is going to be expanded"));

    m_namePattern = wxT("m_treebook");
    SetName(GenerateName());
}

TreeBookWrapper::~TreeBookWrapper() {}

wxString TreeBookWrapper::GetXRCPageClass() const { return wxT("treebookpage"); }

wxcWidget* TreeBookWrapper::Clone() const { return new TreeBookWrapper(); }

void TreeBookWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/treebook.h>")); }

wxString TreeBookWrapper::GetWxClassName() const { return wxT("wxTreebook"); }
