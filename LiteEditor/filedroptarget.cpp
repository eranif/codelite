#include "filedroptarget.h"
#include "manager.h"

FileDropTarget::FileDropTarget()
: wxFileDropTarget()
{
}

FileDropTarget::~FileDropTarget()
{
}

bool FileDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames)
{
	wxUnusedVar(x);
	wxUnusedVar(y);
	for(size_t i=0; i<filenames.GetCount(); i++){
		ManagerST::Get()->OpenFile(filenames.Item(i), wxEmptyString);
	}
	return true;
}

