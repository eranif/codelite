#ifndef LLDBFORMAT_H
#define LLDBFORMAT_H

#include <algorithm>
#include "LLDBEnums.h"
#include "wxStringHash.h"
#include <wx/string.h>
#include <vector>
#include <wx/menu.h>

class LLDBFormat
{
    static std::unordered_map<int, wxString> m_formats;
    static std::unordered_map<int, int> m_formatToMenuId;
    static std::unordered_map<int, int> m_menuIdToFormat;
    static std::vector<wxString> m_formatsVector; // In order

public:
    LLDBFormat();
    virtual ~LLDBFormat();
    
    static void Initialise();
    
    static wxString GetName(eLLDBFormat foramt);
    static size_t GetCount() { return m_formats.size(); }
    /**
     * @brief return the menu ID from the foramt
     */
    static int GetFormatMenuID(eLLDBFormat format);
    /**
     * @brief return the format ID from the menu ID
     */
    static eLLDBFormat GetFormatID(int menuID);
    
    /**
     * @brief Create a menu for the various formats
     */
    static wxMenu* CreateMenu();
};

#endif // LLDBFORMAT_H
