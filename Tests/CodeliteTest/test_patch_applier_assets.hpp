#pragma once

#include <wx/string.h>

inline static wxString patch_content_multiple_hunks1 =
    R"#(--- C:\msys64\home\eran\devl\codelite\Plugin\MarkdownStyler.hpp
+++ C:\msys64\home\eran\devl\codelite\Plugin\MarkdownStyler.hpp
@@ -3,6 +3,7 @@
 #include "clSTCContainerStylerBase.hpp"
 #include "cl_command_event.h"
 #include "codelite_exports.h"
+#include <unordered_set>

 #include <stack>
 #include <wx/event.h>
@@ -24,6 +25,11 @@
     kNumberedListItem,
     kNumberedListItemDot,
     kUrl,
+    kCodeBlockKeyword,
+    kCodeBlockString,
+    kCodeBlockNumber,
+    kCodeBlockFunction,
+    kCodeBlockOperator,
 };

 enum class MarkdownState {
@@ -36,6 +42,7 @@
     kStrong2Text,
     kEmphasis2Text,
     kHeaderText,
+    kCodeBlockContent,
     kUrl,
 };

@@ -48,10 +55,15 @@
     void Reset() override;
     wxString GetUrlFromPosition(int pos);

 private:
     void OnStyle(clSTCAccessor& accessor);
     void OnHostspotClicked(wxStyledTextEvent& event);
+    void StyleCodeBlockContent(clSTCAccessor& accessor);
+    bool IsKeyword(const wxString& word) const;
+    bool IsOperator(wxChar ch) const;
+    void InitKeywords();

     std::stack<MarkdownState> m_states;
+    std::unordered_set<wxString> m_keywords;
 };

 wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_MARKDOWN_LINK_CLICKED, clCommandEvent);
)#";

inline static wxString multiple_hunks1_source_file = R"#(#pragma once

#include "clSTCContainerStylerBase.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <stack>
#include <wx/event.h>
#include <wx/stc/stc.h>

enum MarkdownStyles {
    kDefault,
    kHeader1,
    kHeader2,
    kHeader3,
    kHeader4,
    kHeader5,
    kHeader6,
    kHeaderText,
    kCodeBlockTag,
    kCodeBlockText,
    kCodeWord,
    kStrongText,
    kStrong2Text,
    kStrongTag,
    kStrong2Tag,
    kEmphasisText,
    kEmphasis2Text,
    kEmphasisTag,
    kEmphasis2Tag,
    kBacktick,
    kHorizontalLine,
    kListItem,
    kNumberedListItem,
    kNumberedListItemDot,
    kUrl,
};

enum class MarkdownState {
    kDefault,
    kCodeBlock3,
    kCodeBlock4,
    kCodeBlock5,
    kCodeBlockTag,
    kCodeWord,
    kStrong2Text,
    kEmphasis2Text,
    kHeaderText,
    kUrl,
};

class WXDLLIMPEXP_SDK MarkdownStyler : public clSTCContainerStylerBase
{
public:
    MarkdownStyler(wxStyledTextCtrl* ctrl);
    virtual ~MarkdownStyler();
    void InitStyles() override;
    void Reset() override;
    wxString GetUrlFromPosition(int pos);

private:
    void OnStyle(clSTCAccessor& accessor);
    void OnHostspotClicked(wxStyledTextEvent& event);

    std::stack<MarkdownState> m_states;
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_MARKDOWN_LINK_CLICKED, clCommandEvent);)#";
