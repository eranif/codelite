#ifndef XMLCODECOMPLETION_H
#define XMLCODECOMPLETION_H

#include "smart_ptr.h"

class IEditor;
class XMLCodeCompletion
{
public:
    typedef SmartPtr<XMLCodeCompletion> Ptr_t;
    
public:
    XMLCodeCompletion();
    virtual ~XMLCodeCompletion();
    
    void CodeComplete(IEditor* editor);
};

#endif // XMLCODECOMPLETION_H
