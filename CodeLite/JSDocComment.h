#ifndef JSDOCCOMMENT_H
#define JSDOCCOMMENT_H

#include "JSObject.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL JSDocComment
{
public:
    JSDocComment();
    virtual ~JSDocComment();
    
protected:
    /**
     * @brief try to parse a "@var" and associate it with the JS object
     */
    void ProcessVarDoc(JSObject::Ptr_t obj);
    
    /**
     * @brief parse comment associated with a function
     */
    void ProcessFunction(JSObject::Ptr_t func);
    
public:
    /**
     * @brief process JS object (redirect to the proper ProcessVarDoc or ProcessFunction)
     * methods
     */
    void Process(JSObject::Ptr_t obj);
};

#endif // JSDOCCOMMENT_H
