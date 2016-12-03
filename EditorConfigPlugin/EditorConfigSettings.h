#ifndef EDITORCONFIGSETTINGS_H
#define EDITORCONFIGSETTINGS_H

#include <wx/string.h>

class EditorConfigSettings
{
    int m_flags;

    enum eFlags {
        kEnabled = (1 << 0),
    };

private:
    void SetFlag(EditorConfigSettings::eFlags flag, bool b)
    {
        if(b) {
            m_flags |= flag;

        } else {
            m_flags &= flag;
        }
    }

public:
    EditorConfigSettings();
    virtual ~EditorConfigSettings();

    bool IsEnabled() const { return m_flags & kEnabled; }
    void SetEnabled(bool b) { SetFlag(kEnabled, b); }
    
    EditorConfigSettings& Load();
    EditorConfigSettings& Save();
};

#endif // EDITORCONFIGSETTINGS_H
