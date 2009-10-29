#ifndef __ACTIVE_TIP_H__
#define __ACTIVE_TIP_H__

#include <wx/panel.h>
#include <vector>
#include <wx/bitmap.h>

extern const wxEventType ACTIVETIP_LINE_EXPANDING;

/**
 * @class ATClientDataBase
 * @author eran
 * @date 10/28/09
 * @file active_tip.h
 * @brief ActiveTip client data base class. Derive your own class from this class
 * to associate user data with a given row
 */
class ATClientData
{
public:
	ATClientData() {}
	virtual ~ATClientData() {}
};

/**
 * @class ATLine
 * @author eran
 * @date 10/28/09
 * @file active_tip.h
 * @brief data structure representing a single line in a single tip window
 */
class ATLine
{
	wxString       m_key;
	wxString       m_value;
	bool           m_isExpandable;
	ATClientData * m_clientData;
	wxRect         m_rect;
	bool           m_active;

public:
	ATLine(const wxString& key, const wxString &value, bool isExpandable, ATClientData *clientData = NULL)
			: m_key          ( key          )
			, m_value        ( value        )
			, m_isExpandable ( isExpandable )
			, m_clientData   ( clientData   )
			, m_active       ( false        ) {}

	ATLine()
			: m_key(wxT(""))
			, m_value(wxT(""))
			, m_isExpandable(false)
			, m_clientData(NULL)
			, m_active(false) {}

	~ATLine();

	void SetClientData(ATClientData* clientData) {
		this->m_clientData = clientData;
	}
	void SetIsExpandable(const bool& isExpandable) {
		this->m_isExpandable = isExpandable;
	}
	void SetKey(const wxString& key) {
		this->m_key = key;
	}
	void SetValue(const wxString& value) {
		this->m_value = value;
	}
	ATClientData* GetClientData() {
		return m_clientData;
	}
	const bool& GetIsExpandable() const {
		return m_isExpandable;
	}
	const wxString& GetKey() const {
		return m_key;
	}
	const wxString& GetValue() const {
		return m_value;
	}
	void SetRect(const wxRect& rect) {
		this->m_rect = rect;
	}
	const wxRect& GetRect() const {
		return m_rect;
	}
	void SetActive(bool active) {
		this->m_active = active;
	}
	bool GetActive() const {
		return m_active;
	}
};

struct ATFrame {
	wxString            m_title;
	std::vector<ATLine> m_lines;
	wxRect              m_rect;
	bool                m_active;

	ATFrame() : m_title(wxT("")), m_active(false) {}
};

typedef std::vector<ATFrame> ATFrames;

class ActiveTip : public wxPanel
{
	ATFrames   m_rects;
	wxBitmap   m_plusBmp;
	wxBitmap   m_minusBmp;
	wxRect     m_collapseRect;

public:
	enum {
		AT_NOWHERE = -1,
		AT_COLLAPSE_BUTTON,
		AT_LINE
	};

protected:
	wxSize DoCalcSize();
	void DoDrawLine(wxDC &dc, ATLine &line);
	void DoUnselectAll();
	int  DoHitTest(const wxPoint &pt, size_t &idx);
	void DoDrawFrame();
public:
	ActiveTip(wxWindow *parent, const ATFrame& frameContent);
	virtual ~ActiveTip();

	DECLARE_EVENT_TABLE()
	void OnPaint(wxPaintEvent &event);
	void OnEraseBg(wxEraseEvent &event);
	void OnMouseLeftDown(wxMouseEvent &event);
	void OnMouseLeaveWindow(wxMouseEvent &event);
	void OnMouseMove(wxMouseEvent &event);
};

class ActiveTipData {
public:
	// new frame to display to the user
	ATFrame       newFrame;
	// clientData as provided by the caller when
	// the line was constructed
	ATClientData* clientData;
	// set this to true if a new frame was provided
	bool          hasNewFrame;

	ActiveTipData()
		: clientData(NULL)
		, hasNewFrame(false)
		{}
};
#endif // __ACTIVE_TIP_H__
