#include "EclipseDiffThemeImporter.h"
#include <wx/colour.h>

EclipseDiffThemeImporter::EclipseDiffThemeImporter() { SetFileExtensions("*.diff;*.patch;Diff;*.Diff"); }

EclipseDiffThemeImporter::~EclipseDiffThemeImporter() {}

LexerConf::Ptr_t EclipseDiffThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, "diff", 16);
    CHECK_PTR_RET_NULL(lexer);

    //
    // Add lexer specific code here
    //
    AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, "1", "Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, "2", "Command", m_klass.colour, m_background.colour);
    AddProperty(lexer, "3", "Header", m_keyword.colour, m_background.colour);
    AddProperty(lexer, "4", "Position", m_keyword.colour, m_background.colour);

    // Use pink and forest green
    if(IsDarkTheme()) {
        AddProperty(lexer, "5", "Line Deleted", "#FF8080", m_background.colour, false, false, true);
        AddProperty(lexer, "6", "Line Added", "#80FF80", m_background.colour, false, false, true);
    } else {
        AddProperty(lexer, "5", "Line Deleted", "RED", "PINK", false, false, true);
        AddProperty(lexer,
                    "6",
                    "Line Added",
                    "DARK GREEN",
                    wxColour("DARK GREEN").ChangeLightness(170).GetAsString(wxC2S_HTML_SYNTAX),
                    false,
                    false,
                    true);
    }
    //
    // Finalize the importer
    //
    FinalizeImport(lexer);
    return lexer;
}
