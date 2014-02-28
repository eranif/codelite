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
    
    // View mode
    enum {
        kViewModeDefault        = 0x00000000,
        kViewSingle             = 0x00000001,
        kViewVerticalSplit      = 0x00000002,
        kViewHorizontalSplit    = 0x00000004,
        kAllViewModes           = (kViewSingle|kViewVerticalSplit|kViewHorizontalSplit),
    };
    
protected:
    size_t m_flags;
    size_t m_viewFlags;
    
public:

    DiffConfig();
    virtual ~DiffConfig();

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;
    
    DiffConfig& SetViewMode(int mode) {
        m_viewFlags &= ~kAllViewModes;
        m_viewFlags |= mode;
        return *this;
    }
    
    bool IsSplitVertical() const {
        if (! (m_viewFlags & kAllViewModes) ) {
            // none of the modes is selected, select the default "Vertical"
            return true;
        }
        return m_viewFlags & kViewVerticalSplit;
    }
    
    bool IsSplitHorizontal() const {
        return m_viewFlags & kViewHorizontalSplit;
    }
    
    bool IsSingleViewMode() const {
        return m_viewFlags & kViewSingle;
    }
    
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
