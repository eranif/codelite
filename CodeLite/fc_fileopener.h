#ifndef __fcfileopener__
#define __fcfileopener__

#include <vector>
#include <string>
#include <set>
#include <cstdio>

class fcFileOpener
{
	std::vector<std::string> _searchPath  ;
	std::set<std::string>    _matchedfiles;
	std::set<std::string>    _scannedfiles;
	static fcFileOpener*     ms_instance  ;
	int                      _depth       ;
	int                      _maxDepth    ;
private:
	bool IsPathExist(const std::string &path);

public:
	static fcFileOpener* Instance();
	static void Release();

	void AddSearchPath(const std::string &path);
	FILE *OpenFile(const std::string &include_path);

	void ClearResults()    {
		_matchedfiles.clear();
		_scannedfiles.clear();
		_depth = 0;
	}

	void ClearSearchPath() {
		_searchPath.clear();
	}

	void incDepth() {
		_depth++;
	}

	void decDepth() {
		_depth--;
		if ( _depth < 0) {
			_depth = 0;
		}
	}

	int  getDepth() {
		return _depth;
	}

	void setMaxDepth(const int& _maxDepth) {
		this->_maxDepth = _maxDepth;
	}

	const int& getMaxDepth() const {
		return _maxDepth;
	}

	// getters
	const std::set<std::string>& GetResults() const {
		return _matchedfiles;
	}
private:
	fcFileOpener();
	virtual ~fcFileOpener();


};
#endif // __fcfileopener__
