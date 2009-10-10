#include "fc_fileopener.h"
fcFileOpener* fcFileOpener::ms_instance = 0;

fcFileOpener::fcFileOpener()
{
}

fcFileOpener::~fcFileOpener()
{
}

fcFileOpener* fcFileOpener::Instance()
{
	if(ms_instance == 0){
		ms_instance = new fcFileOpener();
	}
	return ms_instance;
}

void fcFileOpener::Release()
{
	if(ms_instance){
		delete ms_instance;
	}
	ms_instance = 0;
}

void fcFileOpener::AddSearchPath(const std::string& path)
{
	if ( IsPathExist(path) ) {
		return;
	}

	_searchPath.push_back(path);
}

bool fcFileOpener::IsPathExist(const std::string& path)
{
	for(size_t i=0; i<_searchPath.size(); i++){
		if(_searchPath.at(i) == path){
			return true;
		}
	}
	return false;
}

FILE* fcFileOpener::OpenFile(const std::string& include_path)
{
	if ( include_path.empty() ) {
		return NULL;
	}

	std::string mod_path ( include_path );
	static std::string trimString("\"<> \t");

	mod_path.erase(0, mod_path.find_first_not_of(trimString));
	mod_path.erase(mod_path.find_last_not_of    (trimString)+1);

	if ( _scannedfiles.find(mod_path) != _scannedfiles.end() ) {
		// we already scanned this file
		return NULL;
	}

	// try to open the file as is
	FILE *fp = fopen(mod_path.c_str(), "r");
	if ( fp ) {
		_matchedfiles.insert( mod_path );
		_scannedfiles.insert( mod_path );
		return fp;
	}

	// try to prepend the search paths
	for(size_t i=0; i<_searchPath.size(); i++){
		fp = fopen(std::string(_searchPath.at(i) + "/" + mod_path).c_str(), "r" );
		if ( fp ) {
			_matchedfiles.insert( std::string(_searchPath.at(i) + "/" + mod_path) );
			_scannedfiles.insert( mod_path );
			return fp;
		}
	}
	_scannedfiles.insert( mod_path );
	return NULL;
}
