#ifndef __fcfileopener__
#define __fcfileopener__

#include <vector>
#include <string>
#include <set>
#include <stdio.h>

class fcFileOpener
{
	static fcFileOpener*     ms_instance        ;

	std::vector<std::string> _searchPath        ;
	std::vector<std::string> _excludePaths      ;
	std::set<std::string>    _matchedfiles      ;
	std::set<std::string>    _scannedfiles      ;
	int                      _depth             ;
	int                      _maxDepth          ;
	std::set<std::string>    _namespaces        ;
	std::set<std::string>    _namespaceAliases  ;
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
		_namespaces.clear();
		_namespaceAliases.clear();
		_depth = 0;
	}

	void ClearSearchPath() {
		_searchPath.clear();
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

	////////////////////////////////////////////////////
	// Using namespace support
	const std::set<std::string>& GetNamespaces() const {
		return _namespaces;
	}

	void ClearNamespace(){_namespaces.clear();}
	void AddNamespace(const char* ns);

	////////////////////////////////////////////////////
	// Namespace aliasing
	const std::set<std::string>& GetNamespaceAliases() const {
		return _namespaceAliases;
	}
	void ClearNamespaceAliases(){_namespaceAliases.clear();}
	void AddNamespaceAlias(const char* alias){_namespaceAliases.insert(alias);}

private:
	fcFileOpener();
	virtual ~fcFileOpener();


};
#endif // __fcfileopener__
