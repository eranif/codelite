#ifndef PHPENTITYFUNCTION_H
#define PHPENTITYFUNCTION_H

#include "codelite_exports.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase

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

    // The function signature
    PHPEntityBase::Ptr_t m_signature;

    // The function name
    wxString m_name;

    // Function flags
    size_t m_flags;

public:
    virtual wxString ID() const;
    PHPEntityFunction();
    virtual ~PHPEntityFunction();

    // Accessors
    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }
    void SetFlags(size_t flags);
    bool Is(eFunctionFlags flag) const { return m_flags & flag; }
    void SetReturnValue(PHPEntityBase::Ptr_t returnValue) { this->m_returnValue = returnValue; }
    void SetSignature(PHPEntityBase::Ptr_t signature) { this->m_signature = signature; }
    PHPEntityBase::Ptr_t GetReturnValue() const { return m_returnValue; }
    PHPEntityBase::Ptr_t GetSignature() const { return m_signature; }
};

#endif // PHPENTITYFUNCTION_H
