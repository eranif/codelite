#include "splitter_window_page.h"
#include "allocator_mgr.h"
#include "splitter_window_wrapper.h"

SplitterWindowPage::SplitterWindowPage()
{
    SetType(ID_WXSPLITTERWINDOW_PAGE);
    m_namePattern = wxT("m_splitterPage");
    SetName(GenerateName());
}

SplitterWindowPage::~SplitterWindowPage() {}

wxcWidget* SplitterWindowPage::Clone() const { return new SplitterWindowPage(); }

wxString SplitterWindowPage::CppCtorCode() const
{
    wxString code = PanelWrapper::CppCtorCode();
    SplitterWindowWrapper* sp = dynamic_cast<SplitterWindowWrapper*>(GetParent());
    if(!sp) { return code; }

    // Add the splitter specific code here
    const wxcWidget::List_t& siblings = sp->GetChildren();
    if(IsLastChild()) {
        // We are the last child, create the code that calls the 'split' method
        wxString first, second;
        wxcWidget::List_t::const_iterator iter = siblings.begin();
        for(; iter != siblings.end(); iter++) {
            if(first.IsEmpty()) {
                first = (*iter)->GetName();

            } else {
                second = (*iter)->GetName();
            }
        }

        bool splitVert = sp->IsSplitVertically();
        if(first.IsEmpty() == false && second.IsEmpty() == false) {
            code << sp->GetName();

            if(splitVert) {
                code << wxT("->SplitVertically(");
            } else {
                code << wxT("->SplitHorizontally(");
            }

            code << first << wxT(", ") << second << wxT(", ") << sp->GetSashPos() << wxT(");\n");
        }
    }
    return code;
}
