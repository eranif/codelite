#ifndef CLSTRINGVIEW_HPP
#define CLSTRINGVIEW_HPP

#include <codelite_exports.h>
#include <wx/string.h>

class WXDLLIMPEXP_CL clStringView
{
    const wxChar* m_pdata = nullptr;
    size_t m_length = 0;

private:
    // No copy
    clStringView& operator=(const clStringView& other);
    clStringView(const clStringView& other);

public:
    ~clStringView();
    clStringView();
    clStringView(const wxString& buffer)
        : clStringView(buffer.c_str(), buffer.length())
    {
    }
    clStringView(const wxChar* p, size_t len);

    /**
     * @brief construct a string object from this view
     */
    wxString MakeString() const;

    const wxChar* data() const { return m_pdata; }
    size_t length() const { return m_length; }
    wxChar operator[](size_t index) const { return m_pdata[index]; }
    bool empty() const { return m_length == 0; }
    /**
     * @brief try to shift the buffer to the right by `count` chars
     * @return true if the buffer length >= count, false otherwise
     */
    bool Advance(size_t count);
    /**
     * @brief find `what`. return the index or wxNOT_FOUND if no match is found
     */
    int Find(const wxString& what) const;

    /**
     * @brief reset the view with a new content
     */
    void Reset(const wxString& buffer);
};

#endif // CLSTRINGVIEW_HPP
