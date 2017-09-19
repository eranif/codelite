#include "PHPSetterGetterEntry.h"
#include <wx/tokenzr.h>
#include "PHPEntityVariable.h"

PHPSetterGetterEntry::PHPSetterGetterEntry() {}

PHPSetterGetterEntry::PHPSetterGetterEntry(PHPEntityBase::Ptr_t entry)
    : m_entry(entry)
{
}

PHPSetterGetterEntry::~PHPSetterGetterEntry() {}

wxString PHPSetterGetterEntry::GetGetter(size_t flags) const
{
    wxString nameNoDollar = m_entry->Cast<PHPEntityVariable>()->GetNameNoDollar();
    wxString nameWithDollar = m_entry->GetShortName();

    wxString prefix;
    if(!(flags & kSG_NoPrefix)) {
        prefix = (flags & kSG_StartWithLowercase) ? "get" : "Get";
        if(m_entry->Cast<PHPEntityVariable>()->IsBoolean()) {
            // A boolean member, use "is" as the prefix for the getter
            prefix = (flags & kSG_StartWithLowercase) ? "is" : "Is";
        }
    }

    // Remove user prefixes
    wxString functionName = nameNoDollar;
    FormatName(functionName, flags);

    functionName.Prepend(prefix);
    if(flags & kSG_NameOnly) {
        return functionName;
    }

    wxString body;
    body << "\n"
         << "    /**\n"
         << "     * @return " << m_entry->Cast<PHPEntityVariable>()->GetTypeHint() << "\n"
         << "     */\n"
         << "    public function " << functionName << "()\n"
         << "    {\n"
         << "        return $this->" << nameNoDollar << ";\n"
         << "    }";
    return body;
}

wxString PHPSetterGetterEntry::GetSetter(const wxString& scope, size_t flags) const
{
    wxString nameNoDollar = m_entry->Cast<PHPEntityVariable>()->GetNameNoDollar();
    wxString nameWithDollar = m_entry->GetShortName();

    // Remove user prefixes
    wxString functionName = nameNoDollar;
    FormatName(functionName, flags);
    wxString prefix;
    prefix = (flags & kSG_StartWithLowercase) ? "set" : "Set";

    functionName.Prepend(prefix);
    if(flags & kSG_NameOnly) {
        return functionName;
    }

    wxString body;
    body << "\n"
         << "    /**\n"
         << "     * @param " << m_entry->Cast<PHPEntityVariable>()->GetTypeHint() << " " << m_entry->GetShortName()
         << "\n";

    if(flags & kSG_ReturnThis) {
        body << "     *\n";
        body << "     * @return " << scope << "\n";
    }
    body << "     */\n"
         << "    public function " << functionName << "(" << nameWithDollar << ")\n"
         << "    {\n"
         << "        $this->" << nameNoDollar << " = " << nameWithDollar << ";\n";
    if(flags & kSG_ReturnThis) {
        body << "\n"
             << "        return $this;\n";
    }
    body << "    }";
    return body;
}

void PHPSetterGetterEntry::FormatName(wxString& name, size_t flags) const
{
    if(name.StartsWith(wxT("m_"))) {
        name = name.Mid(2);

    } else if(name.StartsWith(wxT("_"))) {
        name = name.Mid(1);
    }

    bool startWithUppercase = !(flags & kSG_StartWithLowercase);
    bool usingPrefix = !(flags & kSG_NoPrefix);

    wxStringTokenizer tkz(name, wxT("_"));
    name.Clear();
    size_t count = 0;
    while(tkz.HasMoreTokens()) {
        wxString token = tkz.NextToken();
        wxString pre = token.Mid(0, 1);
        token.Remove(0, 1);
        if(usingPrefix) {
            pre.MakeUpper();

        } else {
            // No prefix
            if(startWithUppercase && count == 0) {
                pre.MakeUpper();
            } else if(count) {
                pre.MakeUpper();
            }
        }

        token.Prepend(pre);
        name << token;
        ++count;
    }
}
