#ifndef DIFFCONFIG_H
#define DIFFCONFIG_H

#include "cl_config.h" // Base class: clConfigItem
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK DiffConfig : public clConfigItem
{
public:
    enum {
        kCopyLeftToRightAndMove = 0x00000001,
        kCopyRightToLeftAndMove = 0x00000002,
    };

protected:
    size_t m_flags;

public:

    DiffConfig();
    virtual ~DiffConfig();

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;
    
    void SetFlags(size_t flags) {
        this->m_flags = flags;
    }
    size_t GetFlags() const {
        return m_flags;
    }
    
    DiffConfig& Load();
    void Save();
};

#endif // DIFFCONFIG_H
