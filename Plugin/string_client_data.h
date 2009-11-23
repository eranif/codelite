#ifndef MYSTRINGCLIENTDATA_H
#define MYSTRINGCLIENTDATA_H

#include <wx/string.h>
#include <wx/clntdata.h>

/**
 * @class MyStringClientData
 * @author Eran
 * @date 11/23/2009
 * @file string_client_data.h
 * @brief provide our own string client data, since compilation is broken under window
 * since wxStringClientData does not provide destructor (virtual)
 */
class MyStringClientData : public wxClientData
{
public:
	MyStringClientData() : m_data() { }
	MyStringClientData( const wxString &data ) : m_data(data) { }
	virtual ~MyStringClientData() {}
	
	void SetData( const wxString &data ) {
		m_data = data;
	}
	const wxString& GetData() const {
		return m_data;
	}

private:
	wxString  m_data;
};

#endif // MYSTRINGCLIENTDATA_H
