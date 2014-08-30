#include "PHPSetterGetterEntry.h"
#include <wx/tokenzr.h>

PHPSetterGetterEntry::PHPSetterGetterEntry()
{
}

PHPSetterGetterEntry::PHPSetterGetterEntry(const PHPEntry& entry)
{
    m_entry = entry;
}

PHPSetterGetterEntry::~PHPSetterGetterEntry()
{
}

wxString PHPSetterGetterEntry::GetGetter(eSettersGettersFlags flags) const
{
    wxString name = m_entry.getName();
    
    // Remove user prefixes
    FormatName( name );
    wxString prefix = (flags & kSG_StartWithUpperCase) ? "Get" : "get";
    name.Prepend( prefix );
    if ( flags & kSG_NameOnly ) {
        return name;
    }
    
    wxString body;
    body << "/**\n"
         << " * @return $" << m_entry.getName() << "\n"
         << " */\n"
         << "public function " << name << "() {\n"
         << "    return $this->" << m_entry.getName() << ";\n"
         << "}";
    return body;
}

wxString PHPSetterGetterEntry::GetSetter(eSettersGettersFlags flags) const
{
    wxString name = m_entry.getName();
    
    // Remove user prefixes
    FormatName( name );
    wxString prefix = (flags & kSG_StartWithUpperCase) ? "Set" : "set";
    name.Prepend( prefix );
    if ( flags & kSG_NameOnly ) {
        return name;
    }
    
    wxString body;
    body << "/**\n"
         << " * @param $" << m_entry.getName() << "\n"
         << " */\n"
         << "public function " << name << "( $" << m_entry.getName() << " ) {\n"
         << "    $this->" << m_entry.getName() << " = $" << m_entry.getName() << ";\n"
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
    while (tkz.HasMoreTokens()) {
        wxString token = tkz.NextToken();
        wxString pre = token.Mid(0, 1);
        token.Remove(0, 1);
        pre.MakeUpper();
        token.Prepend(pre);
        name << token;
    }
}

