#include "VirtualDirectoryColour.h"

#include <algorithm>

FolderColour::FolderColour() {}

FolderColour::~FolderColour() {}

void FolderColour::SortToList(const FolderColour::Map_t& m, FolderColour::List_t& l)
{
    l.clear();
    std::for_each(m.begin(), m.end(), [&](const FolderColour::Map_t::value_type& p) { l.push_back(p.second); });

    l.sort([&](const FolderColour& first, const FolderColour& second) {
        return first.GetPath().Cmp(second.GetPath()) > 0;
    });
}

const FolderColour& FolderColour::FindForPath(const FolderColour::List_t& sortedList, const wxString& path)
{
    static FolderColour nullValue;
    FolderColour::List_t::const_iterator iter = std::find_if(
        sortedList.begin(), sortedList.end(), [&](const FolderColour& vdc) { return path.StartsWith(vdc.GetPath()); });
    if(iter == sortedList.end()) {
        return nullValue;
    } else {
        return (*iter);
    }
}
