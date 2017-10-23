#ifndef WXSTRING_HASH
#define WXSTRING_HASH

#include <unordered_set>
#include <unordered_map>

#if wxVERSION_NUMBER < 3100
#include <wx/string.h>
#include <functional>

namespace std
{
    template <> 
    struct hash<wxString> 
    {
        std::size_t operator()(const wxString& s) const { return hash<std::wstring>{}(s.ToStdWstring()); }
    };
}
#endif
#endif // WXSTRING_HASH
