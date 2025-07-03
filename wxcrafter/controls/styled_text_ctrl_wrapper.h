#ifndef STYLEDTEXTCTRLWRAPPER_H
#define STYLEDTEXTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase
#include <map>

class StyledTextCtrlWrapper : public wxcWidget
{
public:
    using LexersMap_t = std::map<wxString, int>;

protected:
    static LexersMap_t m_lexers;
    wxArrayString m_wrapOptions;
    wxArrayString m_eolMode;
    wxArrayString m_indentGuides;

protected:
    wxArrayString GetLexers() const;

public:
    StyledTextCtrlWrapper();
    ~StyledTextCtrlWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // STYLEDTEXTCTRLWRAPPER_H
