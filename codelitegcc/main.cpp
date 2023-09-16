#include "proc.hpp"
#include "string_utils.hpp"
#include "tinyjson.hpp"

#include <fstream>
#include <map>
#include <sstream>
#include <utility>

struct entry {
    std::string file;
    std::string command;
    std::string dir;

    entry(const entry& other) = delete;
    entry(const entry&& other)
    {
        file = std::move(other.file);
        command = std::move(other.command);
        dir = std::move(other.dir);
    }

    entry() {}
    static entry from(const tinyjson::element& e)
    {
        entry d;
        e["file"].as_str(&d.file);
        e["command"].as_str(&d.command);
        e["directory"].as_str(&d.dir);
        return std::move(d);
    }
};

/// Convert the file pointed by `CL_COMPILATION_DB` into a valid
/// `compile_commands.json` file
void finalize_and_exit()
{
    const char* pdb = ::getenv("CL_COMPILATION_DB");
    if(!pdb) {
        std::cerr << "ERROR: environment variable `CL_COMPILATION_DB` is not set" << std::endl;
        exit(EXIT_FAILURE);
    }

    // open the file and read it line by line
    std::ifstream file(pdb);
    if(!file.is_open()) {
        std::cerr << "ERROR: could not open file: " << pdb << std::endl;
        exit(EXIT_FAILURE);
    }

    // build the json
    std::string line;
    std::stringstream ss;
    ss << "[\n";
    bool prepend_comma = false;
    while(!file.eof()) {
        std::getline(file, line);

        if(trim(line).empty()) {
            continue;
        }

        if(prepend_comma) {
            ss << ",";
        }

        ss << line << "\n";
        prepend_comma = true;
    }
    ss << "]\n";
    file.close();

    // load the string and build a tinyjson array
    tinyjson::element root;
    if(!tinyjson::parse(ss.str(), &root)) {
        std::cerr << "ERROR: failed to parse JSON file!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // remove duplicate entries
    // keeping the last seen line
    std::map<std::string, entry> M;
    for(size_t i = 0; i < root.size(); ++i) {
        const auto& d = root[i];
        std::string filename;
        d["file"].as_str(&filename);
        if(M.count(filename)) {
            M.erase(filename);
        }
        M.insert({ filename, entry::from(d) });
    }

    // build new json from the map
    tinyjson::element unique_json;
    tinyjson::element::create_array(&unique_json);
    for(const auto& [filename, d] : M) {
        unique_json.add_array_object()
            .add_property("file", d.file)
            .add_property("command", d.command)
            .add_property("directory", d.dir);
    }
    
    // format it
    std::stringstream as_string;
    tinyjson::to_string(unique_json, as_string);

    std::ofstream out_file{ "compile_commands.json" };
    if(!out_file.is_open()) {
        std::cerr << "ERROR: could not open file: compile_commands.json for write" << std::endl;
        exit(EXIT_FAILURE);
    }
    out_file << as_string.str();
    out_file.flush();
    out_file.close();

    std::cout << "file compile_commands.json created successfully" << std::endl;
    exit(EXIT_SUCCESS);
}

/// Print usage and exit
void print_help_and_exit()
{
    std::string help_string = R"(
    codelite-cc: a helper program that wraps your compiler
    and records the command line in a format acceptable by
    clangd (i.e. `compile_commands.json`)
    
    The compile commands are saved to the file pointed by the
    `CL_COMPILATION_DB` environment variable
    
    Example usage:
    
    # export the required env vars
    export CL_COMPILATION_DB=/tmp/intermediate_compile_commands.log
    
    # tell your Makefile that we want a different compiler
    export CC="/usr/bin/codelite-cc gcc"
    export CXX="/usr/bin/codelite-cc g++"
    
    # build as you usually do
    make -j$(nproc)
    
    # convert the output file pointed by `CL_COMPILATION_DB` into a valid
    # `compile_commands.json`
    /usr/bin/codelite-cc --finalize
    
    # You should now have a `compile_commands.json` to make `clangd` happy
)";
    std::cout << help_string << std::endl;
    exit(EXIT_SUCCESS);
}

// A thin wrapper around gcc that intercepts the compiler (e.g. gcc) command line arguments, store them in a file and
// then invoke the compiler
int main(int argc, char** argv)
{
    // We require at least one argument
    if(argc < 2) {
        exit(EXIT_FAILURE);
    }

    if(strcmp(argv[1], "--help") == 0) {
        print_help_and_exit();
    }

    if(strcmp(argv[1], "--finalize") == 0) {
        finalize_and_exit();
    }

    StringVec_t file_names;
    const char* pdb = getenv("CL_COMPILATION_DB");
    if(!pdb) {
        std::cerr << "WARNING: missing environment variable CL_COMPILATION_DB" << std::endl;
    }

    std::string commandline;
    for(int i = 1; i < argc; ++i) {
        // Wrap all arguments with spaces with double quotes
        std::string arg = argv[i];
        std::string file_name;

        if(is_source_file(arg, file_name)) {
            file_names.push_back(file_name);
        }

        // re-escape double quotes if needed
        size_t pos = arg.find('"');
        while(pos != std::string::npos) {
            arg.replace(pos, 1, "\\\""); // replace it with escapted slash
            pos = arg.find('"', pos + 2);
        }

        if(arg.find(' ') != std::string::npos) {
            std::string a = "\"";
            a += arg;
            a += '"';
            arg.swap(a);
        }
        commandline += arg + " ";
    }

    if(pdb) {
        // only write lines that are not meant for dependencies generation
        char cwd[1024];
        memset(cwd, 0, sizeof(cwd));
        ::getcwd(cwd, sizeof(cwd));

        for(size_t i = 0; i < file_names.size(); ++i) {
            std::string logfile = pdb;

            tinyjson::element obj;
            tinyjson::element::create_object(&obj);
            obj.add_property("directory", cwd)
                .add_property("file", trim(file_names[i]))
                .add_property("command", trim(commandline));

            std::stringstream ss;
            tinyjson::to_string(obj, ss, false);
            file_write_content(logfile, ss.str());
        }
    }

    // launch the real command
    return run_child_process(argc, argv, commandline);
}
