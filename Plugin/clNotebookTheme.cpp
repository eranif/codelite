#include "clNotebookTheme.h"

clNotebookTheme clNotebookTheme::GetTheme(clNotebookTheme::eNotebookTheme theme)
{
    clNotebookTheme notebookTheme;
    switch(theme) {
    case kDefault:
        notebookTheme.m_penColour = wxColour("rgb(98, 98, 98)");
        notebookTheme.m_activeTabPenColour = wxColour("rgb(98, 98, 98)");
        notebookTheme.m_innerPenColour = wxColour("rgb(207, 207, 207)");
        notebookTheme.m_bgColour = wxColour("rgb(153, 153, 153)");
        notebookTheme.m_activeTabTextColour = *wxBLACK;
        notebookTheme.m_tabTextColour = *wxBLACK;
        notebookTheme.m_tabBgColour = wxColour("rgb(178, 178, 178)");
        notebookTheme.m_activeTabBgColour = *wxWHITE; // Default colour is white
        break;
    case kDark:
        notebookTheme.m_penColour = *wxBLACK;//wxColour("rgb(60, 61, 56)");
        notebookTheme.m_activeTabPenColour = wxColour("rgb(60, 61, 56)");
        notebookTheme.m_innerPenColour = wxColour("rgb(102, 102, 100)");
        notebookTheme.m_bgColour = wxColour("rgb(22, 23, 19)");
        notebookTheme.m_activeTabTextColour = *wxWHITE;
        notebookTheme.m_tabTextColour = wxColour("rgb(193, 193, 191)");
        notebookTheme.m_tabBgColour = wxColour("rgb(61, 61, 58)");
        notebookTheme.m_activeTabBgColour = wxColour("rgb(44, 45, 39)");
        break;
    }
    return notebookTheme;
}
