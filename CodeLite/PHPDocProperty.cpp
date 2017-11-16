#include "PHPDocProperty.h"
#include "PHPSourceFile.h"
#include <wx/tokenzr.h>

PHPDocProperty::PHPDocProperty(PHPSourceFile& sourceFile, const wxString& comment)
    : m_sourceFile(sourceFile)
    , m_comment(comment)
{
}

PHPDocProperty::~PHPDocProperty() {}

const PHPDocProperty::Tuple_t& PHPDocProperty::ParseParams()
{
    m_params.clear();
    wxArrayString lines = ::wxStringTokenize(m_comment, "\n\r", wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines.size(); ++i) {
        const wxString& line = lines.Item(i);
        size_t offset = 0;
        wxString word;
        while(NextWord(line, offset, word)) {
            if(!word.IsEmpty() && word.StartsWith("@property")) {
                wxString stype, sname, sdesc;
                if(NextWord(line, offset, word)) {
                    stype = word;
                    if(NextWord(line, offset, word)) {
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
    for(size_t i = 0; i < lines.size(); ++i) {
        wxString& line = lines.Item(i);
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

bool PHPDocProperty::NextWord(const wxString& str, size_t& offset, wxString& word)
{
    if(offset == str.size()) { return false; }
    size_t start = wxString::npos;
    for(; offset < str.size(); ++offset) {
        bool isWhitespace = (str[offset] == ' ') || (str[offset] == '\t');
        if(isWhitespace && (start != wxString::npos)) {
            // we found a trailing whitespace
            break;
        } else if(isWhitespace && (start == wxString::npos)) {
            // skip leading whitespace
            continue;
        } else if(start == wxString::npos) {
            start = offset;
        }
    }

    if((start != wxString::npos) && (offset > start)) {
        word = str.Mid(start, offset - start);
        return true;
    }
    return false;
}
