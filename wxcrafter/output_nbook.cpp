#include "output_nbook.h"
#include "event_notifier.h"
#include "top_level_win_wrapper.h"
#include "wxc_project_metadata.h"
#include "wxgui_helpers.h"
#include "wxguicraft_main_view.h"
#include <wx/debug.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/sstream.h>
#include <wx/stc/stc.h>
#include <wx/textctrl.h>
#include <wx/xml/xml.h>

OutputNBook::OutputNBook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                         const wxString& name)
    : wxNotebook(parent, id, pos, size, style, name)
{
}

OutputNBook::~OutputNBook() {}

void OutputNBook::CppPageSelected(wxStyledTextCtrl* cpptext, wxStyledTextCtrl* headertext) const
{
    wxArrayString headers;
    wxString cpp, header;
    wxStringMap_t additionalFiles;

    GUICraftMainPanel::m_MainPanel->GenerateCppOutput(cpp, header, headers, additionalFiles,
                                                      GUICraftMainPanel::kGenCodeForPreview |
                                                          GUICraftMainPanel::kGenCodeSelectionOnly);

    wxString prefix;
    headers = wxCrafter::MakeUnique(headers);
    prefix << wxCrafter::Join(headers, wxT("\n")) << wxT("\n");
    // Prepare the project additional include files
    wxString projectIncludes;
    const wxArrayString& includes = wxcProjectMetadata::Get().GetIncludeFiles();
    for(size_t i = 0; i < includes.GetCount(); i++) {
        projectIncludes << wxT("#include ") << wxCrafter::AddQuotes(includes.Item(i)) << wxT("\n");
    }
    prefix << projectIncludes;

    wxFileName headerFile = wxcProjectMetadata::Get().BaseHeaderFile();
    wxCrafter::MakeAbsToProject(headerFile);
    wxString cppPrefix;
    cppPrefix << wxT("#include \"") << headerFile.GetFullName() << wxT("\"\n");
    cppPrefix << projectIncludes << wxT("\n\n");

    header.Prepend(prefix);
    cpp.Prepend(cppPrefix);

    cpptext->SetReadOnly(false);
    cpptext->SetText(cpp);
    cpptext->SetReadOnly(true);

    headertext->SetReadOnly(false);
    headertext->SetText(header);
    headertext->SetReadOnly(true);
}

void OutputNBook::XrcPageSelected(wxStyledTextCtrl* text) const
{
    wxString output;
    GUICraftMainPanel::m_MainPanel->GenerateXrcOutput(output, true);
    TopLevelWinWrapper::WrapXRC(output);
    // Format the output
    wxStringInputStream str(output);
    wxStringOutputStream out;
    wxXmlDocument doc(str);
    if(!doc.Save(out)) { ::wxMessageBox(output); }

    text->SetReadOnly(false);
    text->SetText(out.GetString());
    text->SetReadOnly(true);
}
