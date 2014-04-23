#ifndef LLDBSETTINGS_H
#define LLDBSETTINGS_H

#include "LLDBEnums.h"
#include <wx/string.h>
#include "json_node.h"

class LLDBSettings
{
    size_t m_arrItems;
    size_t m_stackFrames;
    size_t m_flags;
    wxString m_types;

public:
    LLDBSettings();
    virtual ~LLDBSettings();

    bool IsRaiseWhenBreakpointHit() const {
        return m_flags & kLLDBOptionRaiseCodeLite;
    }

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
    void SetMaxArrayElements(size_t maxArrayElements) {
        this->m_arrItems = maxArrayElements;
    }
    void SetMaxCallstackFrames(size_t maxCallstackFrames) {
        this->m_stackFrames = maxCallstackFrames;
    }
    void SetTypes(const wxString& types) {
        this->m_types = types;
    }
    size_t GetMaxArrayElements() const {
        return m_arrItems;
    }
    size_t GetMaxCallstackFrames() const {
        return m_stackFrames;
    }
    const wxString& GetTypes() const {
        return m_types;
    }
    
    bool IsDebugAsSuperuser() const;
    
    LLDBSettings& Load();
    LLDBSettings& Save();
    
    // Helpers
    static wxString LoadPerspective();
    static void SavePerspective(const wxString &perspective);

    // Serialization API
    JSONElement ToJSON() const;
    void FromJSON( const JSONElement &json );
};

#endif // LLDBSETTINGS_H
