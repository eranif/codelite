#pragma once

#include "codelite_exports.h"
#include "wxcrafter_plugin.h"

class WXDLLIMPEXP_SDK TextViewerDlg : public TextViewerBaseDlg
{
public:
    TextViewerDlg(wxWindow* parent,
                  const wxString& message,
                  const wxString& content,
                  const wxString& content_lexer,
                  const wxString& title = "CodeLite");
    ~TextViewerDlg() override = default;
};
