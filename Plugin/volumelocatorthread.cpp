#ifdef __WXMSW__

#include "volumelocatorthread.h"
#include "wx/volume.h"

VolumeLocatorThread::VolumeLocatorThread(wxEvtHandler *owner)
: wxThread(wxTHREAD_JOINABLE) 
, m_owner(owner)
{
}

VolumeLocatorThread::~VolumeLocatorThread()
{
}

void *VolumeLocatorThread::Entry()
{
	//Locate all volumes in the system
	//when done, send an event to the owner
	//and terminate
	wxArrayString volumnes = wxFSVolume::GetVolumes();
	
	//for convinience, create the result as a ';' separated
	//string and send an event to the main thread
	wxString result;
	for(size_t i=0; i<volumnes.GetCount(); i++){
		result << volumnes.Item(i);
		result << wxT(";");
	}
	
	wxCommandEvent e(wxEVT_THREAD_VOLUME_COMPLETED);
	e.SetString(result);
	m_owner->AddPendingEvent(e);
	return NULL;
}

#endif // MSW

