#ifndef OPTIONS_CONFIG_H
#define OPTIONS_CONFIG_H

#include "wx/string.h"
#include "wx/xml/xml.h"
#include "wx/colour.h"
#include "configuration_object.h"
#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

class WXDLLIMPEXP_LE_SDK OptionsConfig : public ConfObject
{
	bool m_displayFoldMargin;
	bool m_underlineFoldLine;
	wxString m_foldStyle;
	bool m_displayBookmarkMargin;
	wxString m_bookmarkShape;
	wxColour m_bookmarkBgColour;
	wxColour m_bookmarkFgColour;
	bool m_highlightCaretLine;
	bool m_displayLineNumbers;
	bool m_showIndentationGuidelines;
	wxColour m_caretLineColour;
	wxColour m_caretColour;
	
public:
	OptionsConfig(wxXmlNode *node);
	virtual ~OptionsConfig(void);

	//-------------------------------------
	// Setters/Getters
	//-------------------------------------
	bool GetDisplayFoldMargin() const { return m_displayFoldMargin; }
	bool GetUnderlineFoldLine() const { return m_underlineFoldLine; }
	wxString GetFoldStyle() const { return m_foldStyle; }
	bool GetDisplayBookmarkMargin() const { return m_displayBookmarkMargin; }
	wxString GetBookmarkShape() const { return m_bookmarkShape; }
	wxColour GetBookmarkFgColour() const { return m_bookmarkFgColour; }
	wxColour GetBookmarkBgColour() const { return m_bookmarkBgColour; }
	bool GetHighlightCaretLine() const { return m_highlightCaretLine; }
	bool GetDisplayLineNumbers() const { return m_displayLineNumbers; }
	bool GetShowIndentationGuidelines() const { return m_showIndentationGuidelines; }
	wxColour GetCaretLineColour() const { return m_caretLineColour;}

	void SetDisplayFoldMargin(bool b){ m_displayFoldMargin = b; }
	void SetUnderlineFoldLine(bool b){ m_underlineFoldLine = b; }
	void SetFoldStyle(wxString s){ m_foldStyle = s; }
	void SetDisplayBookmarkMargin(bool b){ m_displayBookmarkMargin = b; }
	void SetBookmarkShape(wxString s){ m_bookmarkShape = s; }
	void SetBookmarkFgColour(wxColour c){ m_bookmarkFgColour = c; }
	void SetBookmarkBgColour(wxColour c){ m_bookmarkBgColour = c; }
	void SetHighlightCaretLine(bool b){ m_highlightCaretLine = b; }
	void SetDisplayLineNumbers(bool b){ m_displayLineNumbers = b; }
	void SetShowIndentationGuidelines(bool b){ m_showIndentationGuidelines = b; }
	void SetCaretLineColour(wxColour c) { m_caretLineColour = c;}
	
	//Setters
	void SetCaretColour(const wxColour& caretColour) {this->m_caretColour = caretColour;}
	//Getters
	const wxColour& GetCaretColour() const {return m_caretColour;}
	
	/**
	 * Return an XML representation of this object
	 * \return XML node
	 */
	wxXmlNode *ToXml() const;
};

typedef SmartPtr<OptionsConfig> OptionsConfigPtr;

#endif // OPTIONS_CONFIG_H

