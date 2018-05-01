#include <algorithm>
#include "LLDBFormat.h"
#include <wx/xrc/xmlres.h>

std::unordered_map<int, wxString> LLDBFormat::m_formats;
std::unordered_map<int, int> LLDBFormat::m_formatToMenuId;
std::unordered_map<int, int> LLDBFormat::m_menuIdToFormat;
std::vector<wxString> LLDBFormat::m_formatsVector;

LLDBFormat::LLDBFormat() {}

LLDBFormat::~LLDBFormat() {}

wxString LLDBFormat::GetName(eLLDBForamt foramt)
{
    if(m_formats.count((int)foramt)) { return m_formats[(int)foramt]; }
    return "";
}

int LLDBFormat::GetFormatMenuID(eLLDBForamt format)
{
    if(m_formatToMenuId.count((int)format) == 0) { return wxNOT_FOUND; }
    return m_formatToMenuId[(int)format];
}

eLLDBForamt LLDBFormat::GetFormatID(int menuID)
{
    if(m_menuIdToFormat.count(menuID) == 0) { return eLLDBForamt::kFormatInvalid; }
    return static_cast<eLLDBForamt>(m_menuIdToFormat[menuID]);
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

        m_formats = { { (int)eLLDBForamt::kFormatDefault, "Default" },
                      { (int)eLLDBForamt::kFormatDecimal, "Decimal" },
                      { (int)eLLDBForamt::kFormatHex, "Hex" },
                      { (int)eLLDBForamt::kFormatOctal, "Octal" },
                      { (int)eLLDBForamt::kFormatBinary, "Binary" },
                      { (int)eLLDBForamt::kFormatFloat, "Float" },
                      { (int)eLLDBForamt::kFormatComplex, "Complex" },
                      { (int)eLLDBForamt::kFormatBoolean, "Boolean" },
                      { (int)eLLDBForamt::kFormatBytes, "Bytes" },
                      { (int)eLLDBForamt::kFormatBytesWithASCII, "Bytes (with text)" },
                      { (int)eLLDBForamt::kFormatCString, "C-string" },
                      { (int)eLLDBForamt::kFormatPointer, "Pointer" } };

        // Map between Foramt ID -> wxMenuItem ID
        m_formatToMenuId = { { (int)eLLDBForamt::kFormatDefault, wxXmlResource::GetXRCID("Default") },
                             { (int)eLLDBForamt::kFormatDecimal, wxXmlResource::GetXRCID("Decimal") },
                             { (int)eLLDBForamt::kFormatHex, wxXmlResource::GetXRCID("Hex") },
                             { (int)eLLDBForamt::kFormatOctal, wxXmlResource::GetXRCID("Octal") },
                             { (int)eLLDBForamt::kFormatBinary, wxXmlResource::GetXRCID("Binary") },
                             { (int)eLLDBForamt::kFormatFloat, wxXmlResource::GetXRCID("Float") },
                             { (int)eLLDBForamt::kFormatComplex, wxXmlResource::GetXRCID("Complex") },
                             { (int)eLLDBForamt::kFormatBoolean, wxXmlResource::GetXRCID("Boolean") },
                             { (int)eLLDBForamt::kFormatBytes, wxXmlResource::GetXRCID("Bytes") },
                             { (int)eLLDBForamt::kFormatBytesWithASCII, wxXmlResource::GetXRCID("Bytes (with text)") },
                             { (int)eLLDBForamt::kFormatCString, wxXmlResource::GetXRCID("C-string") },
                             { (int)eLLDBForamt::kFormatPointer, wxXmlResource::GetXRCID("Pointer") } };

        // Map between wxMenuItem ID -> Foramt ID
        std::for_each(
            m_formatToMenuId.begin(), m_formatToMenuId.end(),
            [&](const std::unordered_map<int, int>::value_type& vt) { m_menuIdToFormat[vt.second] = vt.first; });
    }
}
