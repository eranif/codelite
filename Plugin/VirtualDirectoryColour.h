#ifndef VIRTUALDIRECTORYCOLOUR_H
#define VIRTUALDIRECTORYCOLOUR_H

#include <codelite_exports.h>
#include <wx/string.h>
#include <wx/colour.h>

class WXDLLIMPEXP_SDK VirtualDirectoryColour
{
    wxString m_path;
    wxColour m_colour;

public:
    typedef std::list<VirtualDirectoryColour> List_t;
    typedef std::map<wxString, VirtualDirectoryColour> Map_t;

public:
    VirtualDirectoryColour();
    VirtualDirectoryColour(const wxString& path, const wxColour& colour)
        : m_path(path)
        , m_colour(colour)
    {
    }
    virtual ~VirtualDirectoryColour();

    bool IsOk() const { return !m_path.IsEmpty(); }
    VirtualDirectoryColour& SetColour(const wxColour& colour)
    {
        this->m_colour = colour;
        return *this;
    }
    VirtualDirectoryColour& SetPath(const wxString& path)
    {
        this->m_path = path;
        return *this;
    }
    const wxColour& GetColour() const { return m_colour; }
    const wxString& GetPath() const { return m_path; }

    /**
     * @brief sort map and store the result in a list
     */
    static void SortToList(const VirtualDirectoryColour::Map_t& m, VirtualDirectoryColour::List_t& l);

    /**
     * @brief Search the list for best match for a given path
     */
    static const VirtualDirectoryColour& FindForPath(const VirtualDirectoryColour::List_t& sortedList,
                                                     const wxString& path);
};

#endif // VIRTUALDIRECTORYCOLOUR_H
