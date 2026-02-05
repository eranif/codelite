#include "PHPDocProperty.h"

#include "PHPSourceFile.h"
#include "StringUtils.h"

#include <wx/tokenzr.h>

PHPDocProperty::PHPDocProperty(PHPSourceFile& sourceFile, const wxString& comment)
    : m_sourceFile(sourceFile)
    , m_comment(comment)
{
}

const PHPDocProperty::Tuple_t& PHPDocProperty::ParseParams()
{
    m_params.clear();
    wxArrayString lines = ::wxStringTokenize(m_comment, "\n\r", wxTOKEN_STRTOK);
    for (const wxString& line : lines) {
        size_t offset = 0;
        wxString word;
        while (StringUtils::NextWord(line, offset, word)) {
            if (!word.IsEmpty() && word.StartsWith("@property")) {
                wxString stype, sname, sdesc;
                if (StringUtils::NextWord(line, offset, word)) {
                    stype = word;
                    if (StringUtils::NextWord(line, offset, word)) {
                        sname = word;
                        // The remainder is the description
                        sdesc = line.Mid(offset);
                        m_params.push_back(std::make_tuple(m_sourceFile.MakeIdentifierAbsolute(stype), sname, sdesc));
                    }
                }
            }
        }
    }
    return m_params;
}

const PHPDocProperty::Tuple_t& PHPDocProperty::ParseMethods()
{
    m_params.clear();
    wxArrayString lines = ::wxStringTokenize(m_comment, "\n\r", wxTOKEN_STRTOK);
    for (wxString& line : lines) {
        if(line.Contains("@method")) {
            int where = line.Find("@method");
            line = line.Mid(where + 7); // skip "@method"
            line.Replace("\t", " ");
            line.Trim().Trim(false);
        
            // @method [return type] [name]([[type] [parameter]<, …>]) [<description>]
            wxString sig;
            sig = line.AfterFirst('(');
            sig.Prepend('(');
            sig = sig.BeforeFirst(')');
            sig.Append(')');

            line = line.BeforeFirst('(');
            wxArrayString params = ::wxStringTokenize(line, " ", wxTOKEN_STRTOK);
            if((params.size() == 2) && !sig.IsEmpty()) {
                m_params.push_back(std::make_tuple(m_sourceFile.MakeIdentifierAbsolute(params[0]), params[1], sig));
            } else if(params.size() == 1 && !sig.IsEmpty()) {
                m_params.push_back(std::make_tuple(wxEmptyString, params[0], sig));
            }
        }
    }
    return m_params;
}
