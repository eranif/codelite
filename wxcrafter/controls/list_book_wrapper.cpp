#include "list_book_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include <wx/listbook.h>

ListBookWrapper::ListBookWrapper()
    : NotebookBaseWrapper(ID_WXLISTBOOK)
{
    SetPropertyString(_("Common Settings"), "wxListbook");
    PREPEND_STYLE_TRUE(wxLB_DEFAULT);
    PREPEND_STYLE_FALSE(wxLB_LEFT);
    PREPEND_STYLE_FALSE(wxLB_RIGHT);
    PREPEND_STYLE_FALSE(wxLB_TOP);
    PREPEND_STYLE_FALSE(wxLB_BOTTOM);

    RegisterEvent(wxT("wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED"), wxT("wxListbookEvent"),
                  _("The page selection was changed"));
    RegisterEvent(wxT("wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING"), wxT("wxListbookEvent"),
                  _("The page selection is about to be changed. This event can be vetoed"));
    m_namePattern = wxT("m_listbook");
    SetName(GenerateName());
}

ListBookWrapper::~ListBookWrapper() {}

wxString ListBookWrapper::GetXRCPageClass() const { return wxT("listbookpage"); }

wxcWidget* ListBookWrapper::Clone() const { return new ListBookWrapper(); }

void ListBookWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/listbook.h>")); }

wxString ListBookWrapper::GetWxClassName() const { return wxT("wxListbook"); }
