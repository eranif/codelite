#pragma once

#include <wx/aui/auibook.h>
#include <wx/stc/stc.h>

namespace OutputNBook
{
void CppPageSelected(wxAuiNotebook* book, wxStyledTextCtrl* cpptext, wxStyledTextCtrl* headertext);
void XrcPageSelected(wxAuiNotebook* book, wxStyledTextCtrl* text);
}; // namespace OutputNBook

