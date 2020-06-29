#include "PHPDocParam.h"
#include <wx/wxcrt.h>
#include <wx/tokenzr.h>

PHPDocParam::PHPDocParam(PHPSourceFile& sourceFile, const wxString& comment)
    : m_sourceFile(sourceFile)
    , m_comment(comment)
{
}

PHPDocParam::~PHPDocParam() {}

const PHPDocParam::Vec_t& PHPDocParam::Parse()
{
    wxString sname;
    wxString stype;
    m_params.clear();

    wxStringTokenizer tokenizer(m_comment, " \n\r", wxTOKEN_STRTOK);
    while(tokenizer.HasMoreTokens()) {
        wxString word = tokenizer.GetNextToken();

        if(word == "@param") {
            // Next word should be the type
            if(!tokenizer.HasMoreTokens()) {
                break;
            }
            stype = tokenizer.GetNextToken();

            // Next comes the name
            if(!tokenizer.HasMoreTokens()) {
                break;
            }
            sname = tokenizer.GetNextToken();

            // Handle common developer mistake
            if (stype.StartsWith("$") || stype.StartsWith("&")) {
                sname.swap(stype);
            }

            // TODO Support nullable parameters
            if (stype.StartsWith("?")) {
                stype.Remove(0, 1);
            }

            // TODO Support by reference parameters
            if (sname.StartsWith("&")) {
                sname.Remove(0, 1);
            } else if (stype.EndsWith("&")) {
                stype.RemoveLast();
            }

            stype = m_sourceFile.MakeIdentifierAbsolute(stype);

            m_params.push_back(std::make_pair(sname, stype));
        }
    }
    return m_params;
}
