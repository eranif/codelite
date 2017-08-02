#include "VirtualDirectoryColour.h"
#include <algorithm>

VirtualDirectoryColour::VirtualDirectoryColour() {}

VirtualDirectoryColour::~VirtualDirectoryColour() {}

void VirtualDirectoryColour::SortToList(const VirtualDirectoryColour::Map_t& m, VirtualDirectoryColour::List_t& l)
{
    l.clear();
    std::for_each(m.begin(), m.end(),
                  [&](const VirtualDirectoryColour::Map_t::value_type& p) { l.push_back(p.second); });

    l.sort([&](const VirtualDirectoryColour& first, const VirtualDirectoryColour& second) {
        return first.GetPath().Cmp(second.GetPath()) > 0;
    });
}

const VirtualDirectoryColour& VirtualDirectoryColour::FindForPath(const VirtualDirectoryColour::List_t& sortedList,
                                                                  const wxString& path)
{
    static VirtualDirectoryColour nullValue;
    VirtualDirectoryColour::List_t::const_iterator iter =
        std::find_if(sortedList.begin(), sortedList.end(),
                     [&](const VirtualDirectoryColour& vdc) { return path.StartsWith(vdc.GetPath()); });
    if(iter == sortedList.end()) {
        return nullValue;
    } else {
        return (*iter);
    }
}
