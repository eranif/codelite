#include "ai/SopParser.hpp"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

std::optional<SopInfo> SopInfo::Parse(const wxString& text)
{
    auto lines = wxStringTokenize(text, "\n", wxTOKEN_STRTOK);
    // search for the title
    SopInfo result;
    result.full_sop = text;
    while (!lines.empty()) {
        wxString line = lines[0];
        lines.erase(lines.begin());
        line.Trim().Trim(false);
        if (line.StartsWith("# ")) {
            result.title = line.Mid(2);
            break;
        }
    }

    if (result.title.empty()) {
        // could not find the title.
        return std::nullopt;
    }

    bool found_params_section{false};
    while (!lines.empty()) {
        // Scan for "## Parameters"
        wxString line = lines[0];
        lines.erase(lines.begin());
        line.Trim().Trim(false);
        if (line.StartsWith("## Parameters")) {
            found_params_section = true;
            break;
        }
    }

    if (!found_params_section) {
        return std::nullopt;
    }

    // Read all parameters.
    while (!lines.empty()) {
        wxString line = lines[0];
        lines.erase(lines.begin());
        line.Trim(); // From right only.
        if (line.StartsWith("- ")) {
            line = line.Mid(2);
            Param p;
            wxString name = line.BeforeFirst(' ');
            name.Replace("*", "");
            p.desc = line.AfterFirst(' ');

            name.Trim().Trim(false);
            name = name.BeforeFirst(' ');
            p.name = name;
            result.params.push_back(std::move(p));
        } else if (line.StartsWith(" ")) {
            continue;
        } else {
            break;
        }
    }
    return result;
}

wxString SopInfo::to_string() const
{
    wxString s;
    s << title << "\n";
    for (const auto& param : params) {
        s << "- " << param.name << " => " << param.desc << "\n";
    }
    return s;
}
