/**
 * This class is the interface to ctags and SQLite database.
 * It contains various APIs that allows the caller to parse source file(s),
 * store it into the database and return a symbol tree.
 * TagsManager is also responsible for starting ctags processes.
 *
 * Before you use TagsManager, usually you would like to start ctags,
 * this is easily done by writing something like this:
 *
 * @code
 * // Create ctags processes
 * TagsManagerST::Get()->StartCtagsProcess(TagsGlobal);
 * @endcode
 *
 * In the destructor of your main frame it is recommended to call Free() to avoid memory leaks:
 *
 * @code
 * // kill the TagsManager object first it will do the process termination and cleanup
 * TagsManager::Free();
 * @endcode
 *
 * @ingroup CodeLite
 * @version 1.0
 * first version
 *
 * @date 09-01-2006
 *
 * @author Eran
 *
 */