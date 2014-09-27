#include "PHPEntityBase.h"

PHPEntityBase::PHPEntityBase()
    : m_parent(NULL)
    , m_line(0)
    , m_column(0)
{
}

void PHPEntityBase::AddChild(PHPEntityBase::Ptr_t child)
{
    m_children.insert(std::make_pair(child->GetName(), child));
    child->m_parent = this;
}

void PHPEntityBase::RecursivePrintStdout(PHPEntityBase::Ptr_t parent, int indent) { PrintStdout(indent); }

PHPEntityBase::Ptr_t PHPEntityBase::FindChild(const wxString& name, bool tryPrependingDollar) const
{
    PHPEntityBase::Map_t::const_iterator iter = m_children.find(name);
    if(iter != m_children.end()) {
        return iter->second;
    }

    // Could not find an exact match, try prepending
    if(tryPrependingDollar) {
        wxString modName = name;
        if(!modName.StartsWith("$")) {
            modName.Prepend("$");
        }
        iter = m_children.find(modName);
        if(iter != m_children.end()) {
            return iter->second;
        }
    }
    return PHPEntityBase::Ptr_t(NULL);
}
