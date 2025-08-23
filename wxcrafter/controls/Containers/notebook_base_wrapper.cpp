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

wxString NotebookBaseWrapper::CppCtorCode() const
{
    wxString code;
    code << CPPStandardWxCtor("0");

    // Set the window name
    code << GetName() << "->SetName(" << wxCrafter::WXT(GetName()) << ");\n";

    int xx, yy;

    xx = -1;
    yy = -1;
    for (auto child : m_children) {
        NotebookPageWrapper* page = dynamic_cast<NotebookPageWrapper*>(child);
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
        ilName << GetName() << "_il";
        code << "wxImageList* " << ilName << " = new wxImageList(" << xx << ", " << yy << ");\n";
        code << GetName() << "->AssignImageList(" << ilName << ");\n";
    }

    wxString& helperCode = wxcNotebookCodeHelper::Get().Code();
    wxcWidget* tlw = GetTopLevel();
    if(tlw && tlw->IsPropertyChecked(PROP_PERSISTENT)) {
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
    for (auto* child : m_children) {
        NotebookPageWrapper* page = static_cast<NotebookPageWrapper*>(child);

        // Check the children of this page and the page itself
        NotebookPageWrapper* sel = DoGetSelection(page);
        if(sel) {
            return sel;
        }
    }
    return NULL;
}

void NotebookBaseWrapper::SetSelection(wxcWidget* page)
{
    for (auto* child : m_children) {
        NotebookPageWrapper* pageChild = static_cast<NotebookPageWrapper*>(child);
        DoSetSelection(pageChild, page);
    }
}

void NotebookBaseWrapper::DoSetSelection(NotebookPageWrapper* page, wxcWidget* pageToSelect)
{
    page->SetSelected(pageToSelect == page);
    for (auto* child : page->GetChildren()) {
        NotebookPageWrapper* pageChild = dynamic_cast<NotebookPageWrapper*>(child);
        if (pageChild) {
            // another notebook page
            DoSetSelection(pageChild, pageToSelect);
        }
    }
}

NotebookPageWrapper* NotebookBaseWrapper::DoGetSelection(NotebookPageWrapper* page) const
{
    if(page->IsSelected()) {
        return page;
    }
    for (auto* child : page->GetChildren()) {
        NotebookPageWrapper* pageChild = dynamic_cast<NotebookPageWrapper*>(child);
        if (pageChild) {
            NotebookPageWrapper* sel = DoGetSelection(pageChild);
            if (sel) {
                return sel;
            }
        }
    }
    return NULL;
}

int NotebookBaseWrapper::GetPageIndex(const NotebookPageWrapper* page) const
{
    int count(0);

    for (const auto* child : GetChildren()) {
        if (DoGetPageIndex(dynamic_cast<const NotebookPageWrapper*>(child), page, count)) {
            return count;
        }
    }
    return wxNOT_FOUND;
}

bool NotebookBaseWrapper::DoGetPageIndex(const NotebookPageWrapper* page, const NotebookPageWrapper* pageToFind,
                                         int& count) const
{
    if(page == pageToFind) {
        return true;
    }

    count++;
    if(page->GetChildren().empty()) {
        return false;
    }

    for (auto* child : page->GetChildren()) {
        NotebookPageWrapper* pageChild = dynamic_cast<NotebookPageWrapper*>(child);
        if (pageChild) {
            if (DoGetPageIndex(pageChild, pageToFind, count)) {
                return true;
            }
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
    if(page) {
        target = DoGetChildPageAtDepth(page, targetlevel, 0);
    }

    return target;
}

NotebookPageWrapper* NotebookBaseWrapper::DoGetChildPageAtDepth(NotebookPageWrapper* page, size_t targetlevel,
                                                                size_t currentlevel) const
{
    if(currentlevel == targetlevel) {
        return page;
    }

    // The _last_ child notebookpage is the one to check
    const wxcWidget::List_t& children = page->GetChildren();
    wxcWidget::List_t::const_reverse_iterator riter = children.rbegin();
    for(; riter != children.rend(); ++riter) {
        NotebookPageWrapper* child = dynamic_cast<NotebookPageWrapper*>(*riter);
        if(child) {
            return DoGetChildPageAtDepth(child, targetlevel, ++currentlevel);
        }
    }

    return NULL;
}
