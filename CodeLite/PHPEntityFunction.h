#ifndef PHPENTITYFUNCTION_H
#define PHPENTITYFUNCTION_H

#include "codelite_exports.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include <vector>

class WXDLLIMPEXP_CL PHPEntityFunction : public PHPEntityBase
{
public:
    virtual void PrintStdout(int indent) const;
    enum eFunctionFlags {
        kPublic = (1 << 1),
        kPrivate = (1 << 2),
        kProtected = (1 << 3),
        kFinal = (1 << 4),
        kStatic = (1 << 5),
        kAbstract = (1 << 6),
    };

protected:
    // The local variabels defined in this function of type
    // PHPEntityVariable
    PHPEntityBase::List_t m_locals;

    // The function return value
    PHPEntityBase::Ptr_t m_returnValue;

    // Function flags
    size_t m_flags;

    std::vector<PHPEntityBase::Ptr_t> m_childrenVec;

public:
    PHPEntityFunction();
    virtual ~PHPEntityFunction();

    /**
     * @brief format function signature
     */
    wxString FormatSignature() const;

    /**
     * @brief override the base AddChild() we do this because we need
     * to preserve the order of the children
     */
    virtual void AddChild(PHPEntityBase::Ptr_t child);

    /**
     * @brief write this object into the database
     * @param db
     */
    virtual void Store(wxSQLite3Database& db);

    // Accessors
    size_t GetFlags() const { return m_flags; }
    void SetFlags(size_t flags);
    bool Is(eFunctionFlags flag) const { return m_flags & flag; }
    void SetReturnValue(PHPEntityBase::Ptr_t returnValue) { this->m_returnValue = returnValue; }
    PHPEntityBase::Ptr_t GetReturnValue() const { return m_returnValue; }
};

#endif // PHPENTITYFUNCTION_H
