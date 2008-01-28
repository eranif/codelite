#include "colourrequest.h"

ColourRequest::ColourRequest()
{
}

ColourRequest::~ColourRequest()
{
}

void ColourRequest::SetFileToColour(const wxFileName& fileToColour)
{
	this->m_fileToColour = fileToColour;
}

const wxFileName& ColourRequest::GetFileToColour() const
{
	return m_fileToColour;
}
