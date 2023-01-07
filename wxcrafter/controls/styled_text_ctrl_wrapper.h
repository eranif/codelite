#ifndef STYLEDTEXTCTRLWRAPPER_H
#define STYLEDTEXTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase
#include <map>

class StyledTextCtrlWrapper : public wxcWidget
{
public:
    typedef std::map<wxString, int> LexersMap_t;

protected:
    static LexersMap_t m_lexers;
    wxArrayString m_wrapOptions;
    wxArrayString m_eolMode;
    wxArrayString m_indentGuides;

protected:
    wxArrayString GetLexers() const;

public:
    StyledTextCtrlWrapper();
    virtual ~StyledTextCtrlWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // STYLEDTEXTCTRLWRAPPER_H
