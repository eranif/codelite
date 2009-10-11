#ifndef __fcfileopener__
#define __fcfileopener__

#include <vector>
#include <string>
#include <set>
#include <cstdio>

class fcFileOpener {
	std::vector<std::string> _searchPath  ;
	std::set<std::string>    _matchedfiles;
	std::set<std::string>    _scannedfiles;
	static fcFileOpener*     ms_instance  ;

private:
	bool IsPathExist(const std::string &path);

public:
	static fcFileOpener* Instance();
	static void Release();

	void AddSearchPath(const std::string &path);
	FILE *OpenFile(const std::string &include_path);
	void ClearResults()    {_matchedfiles.clear(); _scannedfiles.clear();}
	void ClearSearchPath() {_searchPath.clear();}

	// getters
	const std::set<std::string>& GetResults() const {return _matchedfiles;}
private:
	fcFileOpener();
	virtual ~fcFileOpener();


};
#endif // __fcfileopener__
