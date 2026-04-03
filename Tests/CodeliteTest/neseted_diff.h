#pragma once

#include <wx/string.h>

static inline const wxString kNestedDiffSample = R"RAW(```patch
--- a/README.md
+++ b/README.md
@@ -43,6 +43,8 @@ All you need is **wxWidgets** and **CMake** — no third-party libraries are re
 - **Custom font selection** in the demo application
 - **Mouse selection, copy/paste, and programmatic selection**
 - **Buffered rendering** with wxWidgets drawing APIs
+- **Clickable text/links** with Ctrl+click to detect tokens and emit link events
+- **Double-click word selection** with configurable delimiter characters
 - **Cross-platform view implementation** in `terminal_view.cpp`
 
 ### Platform Support
@@ -98,7 +100,9 @@ Each backend provides:
 #### Terminal Events (`terminal_event.h/cpp`)
 Custom wxWidgets events for terminal-specific notifications:
 - `wxEVT_TERMINAL_TITLE_CHANGED`: Fired when terminal changes its title via OSC sequences
-- `wxEVT_TERMINAL_TERMINATED`: Fired when the shell/process exits
+- `wxEVT_TERMINAL_TERMINATED`: Fired when the shell/process exits  
+- `wxEVT_TERMINAL_TEXT_LINK`: Fired when user Ctrl+clicks on text to select a token/link
+  - The event carries the clicked text string for custom handling (e.g., opening URLs, file paths)
 
 #### Terminal Theme (`terminal_theme.h`)
 Color scheme management:
@@ -108,6 +112,7 @@ Color scheme management:
 - Selection and highlight colors with alpha transparency
 - Cursor color
 - Helper methods for 256-color palette and true color conversion
+- Link/clicked text color for visual feedback
 - Pre-defined dark and light themes
 
 #### Terminal Logger (`terminal_logger.h/cpp`)
@@ -172,6 +177,13 @@ terminal->Bind(wxEVT_TERMINAL_TITLE_CHANGED, [](wxTerminalEvent& evt) {
 terminal->Bind(wxEVT_TERMINAL_TERMINATED, [](wxTerminalEvent& evt) {
     // Clean up or restart
 });
+
+// Handle text link clicks (Ctrl+click on text)
+terminal->Bind(wxEVT_TERMINAL_TEXT_LINK, [](wxTerminalEvent& evt) {
+    wxString clickedText = evt.GetText();
+    // Handle the clicked text - open URL, file, etc.
+    // Example: wxLaunchDefaultBrowser(clickedText);
+});
 ```
 
 ### Sending Input
@@ -211,6 +223,27 @@ terminal->SetUserSelection(col, row, count);
 terminal->ClearUserSelection();
 terminal->ClearMouseSelection();
 ```
+
+### Clickable Text and Links
+
+```cpp
+// Ctrl+click on text automatically detects the token under cursor
+// and fires wxEVT_TERMINAL_TEXT_LINK event with the clicked text
+
+// Configure word delimiters for double-click selection
+terminal->SetWordDelimiters(" \t\r\n()[]{}\"'<>|;"); // default delimiters
+
+// The terminal view highlights clicked text with the theme's link color
+// Visual feedback is provided during hover and click interactions
+
+// Handle link clicks in your application
+terminal->Bind(wxEVT_TERMINAL_TEXT_LINK, [](wxTerminalEvent& evt) {
+    wxString text = evt.GetText();
+    if (text.StartsWith("http://") || text.StartsWith("https://")) {
+        wxLaunchDefaultBrowser(text);
+    }
+    // Add custom handling for file paths, error locations, etc.
+});
+```
 
 ## Demo Application
 
@@ -227,6 +260,7 @@ The included demo application (`main.cpp`) showcases the library features:
   - Programmatic text selection
   - Line content display
   - Direct input sending
+  - Link click event handling with message box display
 - Multiple terminal tabs in a notebook control
 - Automatic terminal title updates
 - Process termination handling that closes tabs and exits when the last one closes
@@ -261,6 +295,9 @@ Additional options:
 | `void ClearUserSelection()` | Clear programmatic selection |
 | `void ClearMouseSelection()` | Clear mouse selection |
 | `std::string Contents() const` | Get flattened terminal content |
+| `void SetWordDelimiters(const wxString& delims)` | Set delimiters for word/token detection |
+| `wxString GetWordDelimiters() const` | Get current word delimiters |
+| `void ClearClickedRange()` | Clear the currently clicked/highlighted text range |
 
 ### TerminalCore Public Methods
 
@@ -277,6 +314,11 @@ Additional options:
 | `size_t Cols() const` | Get viewport width |
 | `wxPoint Cursor() const` | Get current cursor position |
 
+#### Cell Selection and Text Extraction
+
+The `TerminalCore` provides methods to extract text ranges and manage cell selections for implementing
+clickable links and text selection features. See `terminal_core.h` for detailed API documentation.
+
 ## Terminal Capabilities
 
 ### Supported Escape Sequences
```)RAW";
