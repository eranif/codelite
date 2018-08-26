#ifndef CLTABHISTORY_H
#define CLTABHISTORY_H

#include "codelite_exports.h"
#include <vector>
#include <wx/sharedptr.h>
#include <wx/window.h>
#include <algorithm>

class WXDLLIMPEXP_SDK clTabHistory
{
    std::vector<wxWindow*> m_history;

public:
    typedef wxSharedPtr<clTabHistory> Ptr_t;

public:
    clTabHistory() {}
    virtual ~clTabHistory() {}

    void Push(wxWindow* page)
    {
        if(page == NULL) return;
        Pop(page);
        m_history.insert(m_history.begin(), page);
    }

    void Pop(wxWindow* page)
    {
        if(!page) return;
        std::vector<wxWindow*>::iterator iter =
            std::find_if(m_history.begin(), m_history.end(), [&](wxWindow* w) { return w == page; });
        if(iter != m_history.end()) {
            m_history.erase(iter);
        }
    }

    wxWindow* PrevPage()
    {
        if(m_history.empty()) {
            return NULL;
        }
        // return the top of the heap
        return m_history[0];
    }

    /**
     * @brief clear the history
     */
    void Clear() { m_history.clear(); }

    /**
     * @brief return the tabbing history
     * @return
     */
    const std::vector<wxWindow*>& GetHistory() const { return m_history; }
};

#endif // CLTABHISTORY_H
