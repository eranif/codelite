#ifndef WEBTOOLSCONFIG_H
#define WEBTOOLSCONFIG_H

#include "cl_config.h"

class WebToolsConfig : public clConfigItem
{
    size_t m_jsFlags;

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    enum eJSFlags {
        kJSEnableCC = (1 << 0),
        kJSEnableVerboseLogging = (1 << 1),
        kJSLibraryJQuery = (1 << 2),
        kJSLibraryBrowser = (1 << 3),
        kJSLibraryEcma5 = (1 << 4),
        kJSLibraryEcma6 = (1 << 5),
        kJSLibraryChai = (1 << 6),
        kJSLibraryUnderscore = (1 << 7),
        kJSPluginNode = (1 << 8),
        kJSPluginStrings = (1 << 9),
        kJSPluginAngular = (1 << 10),
    };

public:
    WebToolsConfig();
    virtual ~WebToolsConfig();

    void EnableJavaScriptFlag(eJSFlags flag, bool b)
    {
        if(b) {
            m_jsFlags |= flag;
        } else {
            m_jsFlags &= ~flag;
        }
    }
    bool HasJavaScriptFlag(eJSFlags flag) const { return m_jsFlags & flag; }
    
    /**
     * @brief create tern project file content based on the settings
     */
    wxString GetTernProjectFile() const;
    WebToolsConfig& Load();
    WebToolsConfig& Save();
};

#endif // WEBTOOLSCONFIG_H
