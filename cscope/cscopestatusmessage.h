#ifndef __cscopestatusmessage__
#define __cscopestatusmessage__

#include <wx/string.h>
class CScopeStatusMessage
{
	wxString m_message;
	int m_percentage;

public:
	CScopeStatusMessage();
	virtual ~CScopeStatusMessage();

	void SetMessage(const wxString& message) ;
	void SetPercentage(const int& percentage) ;
	const wxString& GetMessage() const ;
	const int& GetPercentage() const ;

};
#endif // __cscopestatusmessage__
