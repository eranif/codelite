#include "PHPSetterGetterEntry.h"
#include <wx/tokenzr.h>
#include "PHPEntityVariable.h"

PHPSetterGetterEntry::PHPSetterGetterEntry() {}

PHPSetterGetterEntry::PHPSetterGetterEntry(PHPEntityBase::Ptr_t entry)
    : m_entry(entry)
{
}

PHPSetterGetterEntry::~PHPSetterGetterEntry() {}

wxString PHPSetterGetterEntry::GetGetter(eSettersGettersFlags flags) const
{
    wxString nameNoDollar = m_entry->Cast<PHPEntityVariable>()->GetNameNoDollar();
    wxString nameWithDollar = m_entry->GetShortName();
    
    // Remove user prefixes
    wxString functionName = nameNoDollar;
    FormatName(functionName);
    wxString prefix = (flags & kSG_StartWithUpperCase) ? "Get" : "get";
    functionName.Prepend(prefix);
    if(flags & kSG_NameOnly) {
        return functionName;
    }

    wxString body;
    body << "/**\n"
         << " * @return " << nameWithDollar << "\n"
         << " */\n"
         << "public function " << functionName << "() {\n"
         << "    return $this->" << nameNoDollar << ";\n"
         << "}";
    return body;
}

wxString PHPSetterGetterEntry::GetSetter(eSettersGettersFlags flags) const
{
    wxString nameNoDollar = m_entry->Cast<PHPEntityVariable>()->GetNameNoDollar();
    wxString nameWithDollar = m_entry->GetShortName();

    // Remove user prefixes
    wxString functionName = nameNoDollar;
    FormatName(functionName);
    wxString prefix = (flags & kSG_StartWithUpperCase) ? "Set" : "set";
    functionName.Prepend(prefix);
    if(flags & kSG_NameOnly) {
        return functionName;
    }

    wxString body;
    body << "/**\n"
         << " * @param $" << m_entry->Cast<PHPEntityVariable>()->GetNameNoDollar() << "\n"
         << " */\n"
         << "public function " << functionName << "(" << nameWithDollar << ") {\n"
         << "    $this->" << nameNoDollar << " = " << nameWithDollar << ";\n"
         << "}";
    return body;
}

void PHPSetterGetterEntry::FormatName(wxString& name) const
{
    if(name.StartsWith(wxT("m_"))) {
        name = name.Mid(2);

    } else if(name.StartsWith(wxT("_"))) {
        name = name.Mid(1);
    }

    wxStringTokenizer tkz(name, wxT("_"));
    name.Clear();
    while(tkz.HasMoreTokens()) {
        wxString token = tkz.NextToken();
        wxString pre = token.Mid(0, 1);
        token.Remove(0, 1);
        pre.MakeUpper();
        token.Prepend(pre);
        name << token;
    }
}
