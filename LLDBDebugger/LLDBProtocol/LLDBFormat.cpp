#include "LLDBFormat.h"
#include <wx/xrc/xmlres.h>

std::unordered_map<int, wxString> LLDBFormat::m_formats;
std::unordered_map<int, int> LLDBFormat::m_formatToMenuId;
std::unordered_map<int, int> LLDBFormat::m_menuIdToFormat;
std::vector<wxString> LLDBFormat::m_formatsVector;

LLDBFormat::LLDBFormat() {}

LLDBFormat::~LLDBFormat() {}

wxString LLDBFormat::GetName(eLLDBFormat foramt)
{
    if(m_formats.count((int)foramt)) { return m_formats[(int)foramt]; }
    return "";
}

int LLDBFormat::GetFormatMenuID(eLLDBFormat format)
{
    if(m_formatToMenuId.count((int)format) == 0) { return wxNOT_FOUND; }
    return m_formatToMenuId[(int)format];
}

eLLDBFormat LLDBFormat::GetFormatID(int menuID)
{
    if(m_menuIdToFormat.count(menuID) == 0) { return eLLDBFormat::kFormatInvalid; }
    return static_cast<eLLDBFormat>(m_menuIdToFormat[menuID]);
}

wxMenu* LLDBFormat::CreateMenu()
{
    wxMenu* menu = new wxMenu();
    std::for_each(m_formatsVector.begin(), m_formatsVector.end(),
                  [&](const wxString& formatName) { menu->Append(wxXmlResource::GetXRCID(formatName), formatName); });
    return menu;
}

void LLDBFormat::Initialise()
{
    if(m_formats.empty()) {
        m_formatsVector = { { "Default" },  { "Decimal" }, { "Hex" },     { "Octal" }, { "Binary" },
                            { "Float" },    { "Complex" }, { "Boolean" }, { "Bytes" }, { "Bytes (with text)" },
                            { "C-string" }, { "Pointer" } };

        m_formats = { { (int)eLLDBFormat::kFormatDefault, "Default" },
                      { (int)eLLDBFormat::kFormatDecimal, "Decimal" },
                      { (int)eLLDBFormat::kFormatHex, "Hex" },
                      { (int)eLLDBFormat::kFormatOctal, "Octal" },
                      { (int)eLLDBFormat::kFormatBinary, "Binary" },
                      { (int)eLLDBFormat::kFormatFloat, "Float" },
                      { (int)eLLDBFormat::kFormatComplex, "Complex" },
                      { (int)eLLDBFormat::kFormatBoolean, "Boolean" },
                      { (int)eLLDBFormat::kFormatBytes, "Bytes" },
                      { (int)eLLDBFormat::kFormatBytesWithASCII, "Bytes (with text)" },
                      { (int)eLLDBFormat::kFormatCString, "C-string" },
                      { (int)eLLDBFormat::kFormatPointer, "Pointer" } };

        // Map between Foramt ID -> wxMenuItem ID
        m_formatToMenuId = { { (int)eLLDBFormat::kFormatDefault, wxXmlResource::GetXRCID("Default") },
                             { (int)eLLDBFormat::kFormatDecimal, wxXmlResource::GetXRCID("Decimal") },
                             { (int)eLLDBFormat::kFormatHex, wxXmlResource::GetXRCID("Hex") },
                             { (int)eLLDBFormat::kFormatOctal, wxXmlResource::GetXRCID("Octal") },
                             { (int)eLLDBFormat::kFormatBinary, wxXmlResource::GetXRCID("Binary") },
                             { (int)eLLDBFormat::kFormatFloat, wxXmlResource::GetXRCID("Float") },
                             { (int)eLLDBFormat::kFormatComplex, wxXmlResource::GetXRCID("Complex") },
                             { (int)eLLDBFormat::kFormatBoolean, wxXmlResource::GetXRCID("Boolean") },
                             { (int)eLLDBFormat::kFormatBytes, wxXmlResource::GetXRCID("Bytes") },
                             { (int)eLLDBFormat::kFormatBytesWithASCII, wxXmlResource::GetXRCID("Bytes (with text)") },
                             { (int)eLLDBFormat::kFormatCString, wxXmlResource::GetXRCID("C-string") },
                             { (int)eLLDBFormat::kFormatPointer, wxXmlResource::GetXRCID("Pointer") } };

        // Map between wxMenuItem ID -> Foramt ID
        std::for_each(
            m_formatToMenuId.begin(), m_formatToMenuId.end(),
            [&](const std::unordered_map<int, int>::value_type& vt) { m_menuIdToFormat[vt.second] = vt.first; });
    }
}
