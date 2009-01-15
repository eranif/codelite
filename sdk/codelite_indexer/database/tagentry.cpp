#include "tagentry.h"
#include <map>
#include "utils.h"

TagEntry::TagEntry(const char *line)
{
	m_line = line;
	this->initialize();
}

TagEntry::~TagEntry()
{
}

void TagEntry::initialize()
{
	std::string pattern, kind;
	std::string strLine = m_line;

	long lineNumber = -1;
	std::map<std::string, std::string> extFields;

	// get the token name
	std::string name = before_first(strLine, '\t');
	strLine	= after_first(strLine, '\t');

	// get the file name
	std::string fileName = before_first(strLine, '\t');
	strLine	= after_first(strLine, '\t');

	// here we can get two options:
	// pattern followed by ;"
	// or
	// line number followed by ;"
	std::string::size_type end = strLine.find(";\"");
	if (end == std::string::npos) {
		// invalid pattern found
		return;
	}

	if (starts_with(strLine, "/^")) {
		// regular expression pattern found
		pattern = strLine.substr(0, end);
		strLine	= strLine.substr(end + 2);
	} else {
		// line number pattern found, this is usually the case when
		// dealing with macros in C++
		pattern = strLine.substr(0, end);
		strLine	= strLine.substr(end + 2);

		// trime
		string_trim(pattern);

		lineNumber = atoi(pattern.c_str());
	}

	//next is the kind of the token
	if (starts_with(strLine, "\t")) {
		strLine	= after_first(strLine, '\t');
	}

	kind = before_first(strLine, '\t');
	strLine	= after_first(strLine, '\t');

	if (strLine.empty() == false) {
		std::vector<std::string> tokens = string_tokenize(strLine, "\t");
		for (size_t i=0; i<tokens.size(); i++) {
			std::string token = tokens.at(i);
			std::string key = before_first(token, ':');
			std::string val = after_first(token, ':');

			string_trim(key);
			string_trim(val);

			if (key == "line" && !val.empty()) {
				lineNumber = atoi(val.c_str());

			} else {
				if (key == "union" || key == "struct") {

					// remove the anonymous part of the struct / union
					if (!starts_with(val, "__anon")) {
						// an internal anonymous union / struct
						// remove all parts of the
						std::vector<std::string> scopeArr;
						std::string tmp, new_val;

						scopeArr = string_tokenize(val, ":");
						for (size_t i=0; i<scopeArr.size(); i++) {
							if (starts_with(scopeArr.at(i), "__anon") == false) {
								tmp += scopeArr.at(i);
								tmp += "::";
							}
						}

						ends_with(tmp, "::", new_val);
						val = new_val;
					}
				}

				extFields[key] = val;
			}
		}
	}

	string_trim(kind);
	string_trim(name);
	string_trim(fileName);
	string_trim(pattern);

	if (kind == "enumerator") {
		// enums are specials, they are not really a scope so they should appear when I type:
		// enumName::
		// they should be member of their parent (which can be <global>, or class)
		// but we want to know the "enum" type they belong to, so save that in typeref,
		// then patch the enum field to lift the enumerator into the enclosing scope.
		// watch out for anonymous enums -- leave their typeref field blank.
		std::map<std::string,std::string>::iterator e = extFields.find("enum");
		if (e != extFields.end()) {
			std::string typeref = e->second;
			e->second = before_last( before_last(e->second, ':'), ':');
			if (! starts_with( after_last(typeref, ':'), "__anon") ) {
				extFields["typeref"] = typeref;
			}
		}
	}
	create(fileName, name, lineNumber, pattern, kind, extFields);
}

void TagEntry::create(const std::string& fileName, const std::string& name, int lineNumber, const std::string& pattern, const std::string& kind, std::map<std::string, std::string>& extFields)
{
	setName( name );
	setLineNumber( lineNumber );
	setKind( kind.empty() ? "<unknown>" : kind );
	setPattern( pattern );
	setFile( fileName );
	m_extFields = extFields;
	std::string path;

	// Check if we can get full name (including path)
	path = getExtField("class");
	if (!path.empty()) {
		updatePath( path ) ;
	} else {
		path = getExtField("struct");
		if (!path.empty()) {
			updatePath( path ) ;
		} else {
			path = getExtField("namespace");
			if (!path.empty()) {
				updatePath( path ) ;
			} else {
				path = getExtField("interface");
				if (!path.empty()) {
					updatePath( path ) ;
				} else {
					path = getExtField("enum");
					if (!path.empty()) {
						updatePath( path ) ;
					} else {
						path = getExtField("union");
						std::string tmpname = after_last(path, ':');
						if (!path.empty()) {
							if (!starts_with(tmpname, "__anon")) {
								updatePath( path ) ;
							} else {
								// anonymouse union, remove the anonymous part from its name
								path = before_last(path, ':');
								path = before_last(path, ':');
								updatePath( path ) ;
							}
						}
					}
				}
			}
		}
	}

	if (!path.empty()) {
		setScope(path);
	} else {
		setScope("<global>");
	}

	// If there is no path, path is set to name
	if ( getPath().empty() )
		setPath( getName() );

	// Get the parent name
	std::vector<std::string> tokens = string_tokenize(getPath(), "::");
	std::string parent;

	(tokens.size() < 2) ? parent = "<global>" : parent = tokens[tokens.size()-2];
	setParent(parent);
}

void TagEntry::updatePath(std::string & path)
{
	if (!path.empty()) {
		std::string name(path);
		name += "::";
		name += getName();
		setPath(name);
	}
}


void TagEntry::toString()
{
	printf("Line                   :%d\n", m_lineNumber);
}
