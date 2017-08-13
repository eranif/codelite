#ifndef VIRTUALDIRECTORYCOLOUR_H
#define VIRTUALDIRECTORYCOLOUR_H

#include <codelite_exports.h>
#include <wx/string.h>
#include <wx/colour.h>
#include <list>
#include <map>

class WXDLLIMPEXP_SDK FolderColour
{
    wxString m_path;
    wxColour m_colour;

public:
    typedef std::list<FolderColour> List_t;
    typedef std::map<wxString, FolderColour> Map_t;

public:
    FolderColour();
    FolderColour(const wxString& path, const wxColour& colour)
        : m_path(path)
        , m_colour(colour)
    {
    }
    virtual ~FolderColour();

    bool IsOk() const { return !m_path.IsEmpty(); }
    FolderColour& SetColour(const wxColour& colour)
    {
        this->m_colour = colour;
        return *this;
    }
    FolderColour& SetPath(const wxString& path)
    {
        this->m_path = path;
        return *this;
    }
    const wxColour& GetColour() const { return m_colour; }
    const wxString& GetPath() const { return m_path; }

    /**
     * @brief sort map and store the result in a list
     */
    static void SortToList(const FolderColour::Map_t& m, FolderColour::List_t& l);

    /**
     * @brief Search the list for best match for a given path
     */
    static const FolderColour& FindForPath(const FolderColour::List_t& sortedList,
                                                     const wxString& path);
};

#endif // VIRTUALDIRECTORYCOLOUR_H
