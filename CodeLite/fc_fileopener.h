#ifndef __fcfileopener__
#define __fcfileopener__

#include <vector>
#include <string>
#include <set>

class fcFileOpener
{
	std::vector<std::string> _searchPath  ;
	std::vector<std::string> _excludePaths;
	std::set<std::string>    _matchedfiles;
	std::set<std::string>    _scannedfiles;
	static fcFileOpener*     ms_instance  ;
	int                      _depth       ;
	int                      _maxDepth    ;
	std::string              _currpath    ;

private:
	bool IsPathExist(const std::string &path);
	bool IsExcludePathExist(const std::string &path);
	static void normalize_path( std::string &path );
	FILE *try_open(const std::string &path, const std::string &name);

public:
	static fcFileOpener* Instance();
	static void Release();

	static std::string extract_path(const std::string &filePath);

	void AddSearchPath(const std::string &path);
	void AddExcludePath(const std::string &path);
	FILE *OpenFile(const std::string &include_path);

	void ClearResults()    {
		_matchedfiles.clear();
		_scannedfiles.clear();
		_currpath.clear();
		_depth = 0;
	}

	void ClearSearchPath() {
		_searchPath.clear();
		_currpath.clear();
		_excludePaths.clear();
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
	void setCurrpath(const std::string& _currpath) {
		this->_currpath = _currpath;
	}
	const std::string& getCurrpath() const {
		return _currpath;
	}
private:
	fcFileOpener();
	virtual ~fcFileOpener();


};
#endif // __fcfileopener__
