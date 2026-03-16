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

// Loose patch format - custom format with @@ but no line numbers
inline static wxString loose_patch_custom_format = R"#(*** Begin Patch
*** Update File: C:\msys64\home\eran\devl\test_workspace\FileManager.cpp
@@
-bool FileManager::deleteFile(const std::string &filepath) {
-  return std::filesystem::remove(filepath);
-}
+bool FileManager::deleteFile(const std::string &filepath) {
+  try {
+    if (!std::filesystem::exists(filepath)) {
+      return false;
+    }
+    return std::filesystem::remove(filepath);
+  } catch (const std::filesystem::filesystem_error &) {
+    return false;
+  }
+}
*** End Patch)#";

// Loose patch with standard format
inline static wxString loose_patch_standard_format = R"#(@@ -10,3 +10,7 @@
 void Function1() {
-    // Old implementation
+    // New implementation
+    int x = 5;
 })#";

// Loose patch with multiple hunks
inline static wxString loose_patch_multiple_hunks = R"#(@@
-#include "old.h"
+#include "new.h"

@@
 void test() {
-    old_function();
+    new_function();
 })#";

// Loose patch with only additions
inline static wxString loose_patch_additions_only = R"#(@@
+#include <iostream>
+#include <vector>
+
+void newFunction() {
+    std::cout << "Hello" << std::endl;
+})#";

// Loose patch with only deletions
inline static wxString loose_patch_deletions_only = R"#(@@
-void oldFunction() {
-    // This will be removed
-})#";

// Loose patch with context lines
inline static wxString loose_patch_with_context = R"#(@@ -1,5 +1,5 @@
 #include <stdio.h>

 int main() {
-    printf("Old\n");
+    printf("New\n");
     return 0;
 })#";

// Loose patch with mixed content and ignored lines
inline static wxString loose_patch_with_ignored_lines = R"#(This is a comment that should be ignored
@@
-old line
+new line
This should also be ignored
@@
 context
-another old
+another new)#";

// Loose patch without proper hunk markers (edge case)
inline static wxString loose_patch_no_hunks = R"#(-This is not in a hunk
+This is also not in a hunk
Just some random text)#";

// Loose patch with empty lines in hunk
inline static wxString loose_patch_with_empty_lines = R"#(@@
 void test() {
-    line1();
+    line2();

 })#";

// Loose patch with just @@ marker
inline static wxString loose_patch_minimal = R"#(@@
-old
+new)#";
