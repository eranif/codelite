#pragma once

#include "cl_editor.h"
#include "clEditorHost.hpp"

#include <wx/window.h>

/// The editor of a page of the main book: the page itself, or the editor that the page hosts (see clEditorHost).
/// nullptr if the page has no editor (an image, the welcome page, ...)
inline clEditor* clEditorFromPage(wxWindow* page)
{
    if (page == nullptr) {
        return nullptr;
    }
    if (auto* editor = dynamic_cast<clEditor*>(page)) {
        return editor;
    }
    if (auto* host = clEditorHostFromPage(page)) {
        // Only clEditor is created for a host
        IEditor* hosted = host->GetHostedEditor();
        return hosted != nullptr ? dynamic_cast<clEditor*>(hosted->GetCtrl()) : nullptr;
    }
    return nullptr;
}
