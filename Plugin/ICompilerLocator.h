#ifndef ICOMPILERLOCATOR_H
#define ICOMPILERLOCATOR_H

#include <compiler.h>
#include <wx/sharedptr.h>
#include <vector>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK ICompilerLocator
{
public:
    typedef wxSharedPtr<ICompilerLocator>        Ptr_t;
    typedef std::vector<ICompilerLocator::Ptr_t> Vect_t;
    typedef std::vector<CompilerPtr>             CompilerVec_t;
    
protected:
    ICompilerLocator::CompilerVec_t m_compilers;

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
