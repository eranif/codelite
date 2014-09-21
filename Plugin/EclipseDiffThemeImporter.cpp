#include "EclipseDiffThemeImporter.h"
#include <wx/colour.h>

EclipseDiffThemeImporter::EclipseDiffThemeImporter() { SetFileExtensions("*.diff;*.patch;Diff;*.Diff"); }

EclipseDiffThemeImporter::~EclipseDiffThemeImporter() {}

bool EclipseDiffThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    wxXmlNode* properties = InitializeImport(eclipseXmlFile, "diff", 16);
    CHECK_PTR_RET_FALSE(properties);

    //
    // Add lexer specific code here
    //
    AddProperty(properties, "0", "Default", m_foreground.colour, m_background.colour);
    AddProperty(properties, "1", "Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(properties, "2", "Command", m_klass.colour, m_background.colour);
    AddProperty(properties, "3", "Header", m_keyword.colour, m_background.colour);
    AddProperty(properties, "4", "Position", m_keyword.colour, m_background.colour);

    // Use pink and forest green
    if(IsDarkTheme()) {
        AddProperty(properties, "5", "Line Deleted", "#FF8080", m_background.colour, false, false, true);
        AddProperty(properties, "6", "Line Added", "#80FF80", m_background.colour, false, false, true);
    } else {
        AddProperty(properties, "5", "Line Deleted", "RED", "PINK", false, false, true);
        AddProperty(properties,
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
    return FinalizeImport(properties);
}
