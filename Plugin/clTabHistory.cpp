#include "clTabHistory.h"

#include <unordered_set>

void clTabHistory::Compact(const std::vector<wxWindow*>& windows)
{
    // convert the list into set
    std::unordered_set<wxWindow*> windows_to_keep{ windows.begin(), windows.end() };

    // Temporary list that holds the history that we want to keep
    std::vector<wxWindow*> new_list;
    new_list.reserve(m_history.size());

    for (auto win : m_history) {
        if (windows_to_keep.count(win)) {
            new_list.push_back(win);
        }
    }
    m_history.swap(new_list);
}
