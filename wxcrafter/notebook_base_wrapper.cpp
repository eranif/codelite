#include "notebook_base_wrapper.h"
#include "allocator_mgr.h"
#include "notebook_page_wrapper.h"
#include "wxc_notebook_code_helper.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include <wx/bookctrl.h>
#include <wx/imaglist.h>

NotebookBaseWrapper::NotebookBaseWrapper(int id)
    : wxcWidget(id)
{
    PREPEND_STYLE_TRUE(wxBK_DEFAULT);
    PREPEND_STYLE_FALSE(wxBK_LEFT);
    PREPEND_STYLE_FALSE(wxBK_RIGHT);
    PREPEND_STYLE_FALSE(wxBK_TOP);
    PREPEND_STYLE_FALSE(wxBK_BOTTOM);
}

NotebookBaseWrapper::~NotebookBaseWrapper() {}

wxString NotebookBaseWrapper::CppCtorCode() const
{
    wxString code;
    code << CPPStandardWxCtor(wxT("0"));

    // Set the window name
    code << GetName() << "->SetName(" << wxCrafter::WXT(GetName()) << ");\n";

    int xx, yy;

    xx = -1;
    yy = -1;
    List_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); iter++) {
        NotebookPageWrapper* page = dynamic_cast<NotebookPageWrapper*>((*iter));
        if(page) {
            wxSize sz = page->GetImageSize();
            xx = wxMax(sz.x, xx);
            yy = wxMax(sz.y, yy);
        }
    }

    // Add image list
    bool isAuiBook = (GetType() == ID_WXAUINOTEBOOK);
    if((xx != -1 && yy != -1) && !isAuiBook) {
        wxString ilName;
        ilName << GetName() << wxT("_il");
        code << wxT("wxImageList* ") << ilName << wxT(" = new wxImageList(") << xx << wxT(", ") << yy << wxT(");\n");
        code << GetName() << wxT("->AssignImageList(") << ilName << wxT(");\n");
    }

    wxString& helperCode = wxcNotebookCodeHelper::Get().Code();
    wxcWidget* tlw = GetTopLevel();
    if(tlw && tlw->IsPropertyChecked(PROP_PERSISTENT) && wxcSettings::Get().IsLicensed()) {
        helperCode << "\n";
        helperCode << wxCrafter::WX29_BLOCK_START();
        helperCode << "if(!wxPersistenceManager::Get().Find(" << GetName() << ")){\n";
        helperCode << "    wxPersistenceManager::Get().RegisterAndRestore(" << GetName() << ");\n";
        helperCode << "} else {\n";
        helperCode << "    wxPersistenceManager::Get().Restore(" << GetName() << ");\n";
        helperCode << "}\n";
        helperCode << "#endif\n";
    }
    return code;
}

void NotebookBaseWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes();

    ChildrenXRC(text, type);
    text << XRCSuffix();
}

NotebookPageWrapper* NotebookBaseWrapper::GetSelection() const
{
    List_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); iter++) {
        NotebookPageWrapper* page = (NotebookPageWrapper*)*iter;

        // Check the children of this page and the page itself
        NotebookPageWrapper* sel = DoGetSelection(page);
        if(sel) { return sel; }
    }
    return NULL;
}

void NotebookBaseWrapper::SetSelection(wxcWidget* page)
{
    List_t::iterator iter = m_children.begin();
    for(; iter != m_children.end(); iter++) {
        NotebookPageWrapper* child = (NotebookPageWrapper*)*iter;
        DoSetSelection(child, page);
    }
}

void NotebookBaseWrapper::DoSetSelection(NotebookPageWrapper* page, wxcWidget* pageToSelect)
{
    page->SetSelected(pageToSelect == page);
    wxcWidget::List_t& list = page->GetChildren();
    wxcWidget::List_t::iterator iter = list.begin();
    for(; iter != list.end(); iter++) {
        NotebookPageWrapper* child = dynamic_cast<NotebookPageWrapper*>(*iter);
        if(child) {
            // another notebook page
            DoSetSelection(child, pageToSelect);
        }
    }
}

NotebookPageWrapper* NotebookBaseWrapper::DoGetSelection(NotebookPageWrapper* page) const
{
    if(page->IsSelected()) return page;

    wxcWidget::List_t& list = page->GetChildren();
    wxcWidget::List_t::const_iterator iter = list.begin();
    for(; iter != list.end(); iter++) {
        NotebookPageWrapper* child = dynamic_cast<NotebookPageWrapper*>(*iter);
        if(child) {
            NotebookPageWrapper* sel = DoGetSelection(child);
            if(sel) { return sel; }
        }
    }
    return NULL;
}

int NotebookBaseWrapper::GetPageIndex(const NotebookPageWrapper* page) const
{
    int count(0);
    const wxcWidget::List_t& list = GetChildren();
    wxcWidget::List_t::const_iterator iter = list.begin();
    for(; iter != list.end(); iter++) {
        if(DoGetPageIndex((const NotebookPageWrapper*)(*iter), page, count)) { return count; }
    }
    return wxNOT_FOUND;
}

bool NotebookBaseWrapper::DoGetPageIndex(const NotebookPageWrapper* page, const NotebookPageWrapper* pageToFind,
                                         int& count) const
{
    if(page == pageToFind) return true;

    count++;
    if(page->GetChildren().empty()) { return false; }

    List_t::const_iterator iter = page->GetChildren().begin();
    for(; iter != page->GetChildren().end(); iter++) {
        NotebookPageWrapper* child = dynamic_cast<NotebookPageWrapper*>(*iter);
        if(child) {
            if(DoGetPageIndex(child, pageToFind, count)) { return true; }
        }
    }
    return false;
}

NotebookPageWrapper* NotebookBaseWrapper::GetLastPage() const
{
    return dynamic_cast<NotebookPageWrapper*>(*GetChildren().rbegin());
}

wxcWidget* NotebookBaseWrapper::GetChildPageAtDepth(size_t targetlevel)
{
    wxcWidget* target = NULL;

    NotebookPageWrapper* page = GetLastPage(); // Find the last page of the book
    if(page) { target = DoGetChildPageAtDepth(page, targetlevel, 0); }

    return target;
}

NotebookPageWrapper* NotebookBaseWrapper::DoGetChildPageAtDepth(NotebookPageWrapper* page, size_t targetlevel,
                                                                size_t currentlevel) const
{
    if(currentlevel == targetlevel) { return page; }

    // The _last_ child notebookpage is the one to check
    const wxcWidget::List_t& children = page->GetChildren();
    wxcWidget::List_t::const_reverse_iterator riter = children.rbegin();
    for(; riter != children.rend(); ++riter) {
        NotebookPageWrapper* child = dynamic_cast<NotebookPageWrapper*>(*riter);
        if(child) { return DoGetChildPageAtDepth(child, targetlevel, ++currentlevel); }
    }

    return NULL;
}
