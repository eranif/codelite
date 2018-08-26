std::set<wxString> s;
std::for_each(s.begin(), s.end(), [&](const wxString& str){
    str.