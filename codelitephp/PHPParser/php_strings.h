#ifndef PHP_STRINGS_H
#define PHP_STRINGS_H

#include <wx/string.h>
#include <wx/frame.h>
#include <wx/app.h>

#define FRAME static_cast<wxFrame*>(static_cast<wxApp*>(wxApp::GetInstance())->GetTopWindow())

namespace PHPStrings {

const wxString PHP_WORKSPACE_EXT        = wxT("phpwsp");
const wxString PHP_WORKSPACE_VIEW_TITLE = wxT("PHP");

};

#endif // PHP_STRINGS_H

