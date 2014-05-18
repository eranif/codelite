#ifndef ICOMPILERLOCATOR_H
#define ICOMPILERLOCATOR_H

#include <compiler.h>
#include <wx/sharedptr.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK ICompilerLocator
{
protected:
    typedef std::vector<CompilerPtr> CompilerVec_t;
    ICompilerLocator::CompilerVec_t m_compilers;

public:
    typedef wxSharedPtr<ICompilerLocator> Ptr_t;

public:
    ICompilerLocator();
    virtual ~ICompilerLocator();
    
    /**
     * @brief locate the compiler
     */
    virtual bool Locate() = 0;
    
    /**
     * @brief return the compiler
     */
    const ICompilerLocator::CompilerVec_t& GetCompilers() const {
        return m_compilers;
    }
};

#endif // ICOMPILERLOCATOR_H
