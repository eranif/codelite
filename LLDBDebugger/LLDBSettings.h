#ifndef LLDBSETTINGS_H
#define LLDBSETTINGS_H

#include "cl_config.h" // Base class: clConfigItem

class LLDBSettings : public clConfigItem
{
    size_t m_flags;
public:
    enum {
        kBreakOnFirstEntry = 0x00000001,
    };
    
public:
    LLDBSettings();
    virtual ~LLDBSettings();
    
    bool HasFlag(int flag) const {
        return m_flags & flag;
    }
    
    void EnableFlag(int flag, bool enable) {
        if ( enable ) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }
    
public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;
    
    LLDBSettings& Load();
    LLDBSettings& Save();
    
    wxString LoadPerspective();
    void SavePerspective(const wxString &perspective);
};

#endif // LLDBSETTINGS_H
