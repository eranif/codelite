#ifndef __colourrequest__
#define __colourrequest__
#include "wx/filename.h"
#include "worker_thread.h"

class ColourRequest : public ThreadRequest
{

	wxFileName m_fileToColour;

public:
	ColourRequest();
	~ColourRequest();

	void SetFileToColour(const wxFileName& fileToColour) ;
	const wxFileName& GetFileToColour() const ;
};
#endif // __colourrequest__
