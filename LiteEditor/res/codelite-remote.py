#!/usr/bin/python3
import sys
import os
import json
import re

#
# Sample usage:
#   {"command":"ls", "file_extensions":[".cpp",".hpp",".h"], "root_dir":"/c/src/codelite"}
#   {"command":"find", "file_extensions": [".cpp",".hpp",".h"], "root_dir": "/c/src/codelite", "find_what": "wxcReplyEventData"}
#   {"command":"exec", "program": ["/usr/bin/ls"], "wd": "/c/src/codelite", "env": [["ENV1", "ENV_VALUE1"], ["ENV2", "ENV_VALUE2"]]}
#
def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def _get_list_of_files(cmd):
    """
    helper method to on_find_files - return list of files as array    
    """
    root_dir = cmd['root_dir']
    if not os.path.isdir(root_dir):
        eprint("error: {} is not a directory".format(root_dir))
        return None
    
    # build the extension tuple
    exts_set = set()
    for ext in cmd['file_extensions']:
        exts_set.add(ext)
    
    files_arr = []
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            # get the extension
            base_name, curext = os.path.splitext(file)
            if curext in exts_set:
                files_arr.append(os.path.join(root, file))
    return files_arr

def on_exec(cmd):
    pass

def on_find_files(cmd):
    """
    Find list of files with a given extension and from a given root directory    
    
    Example command:
    
    {"command":"ls", "file_extensions":[".cpp",".hpp",".h"], "root_dir":"/c/src/codelite"}
    """
    arr_files = _get_list_of_files(cmd)
    if arr_files is not None:
        print(json.dumps(arr_files))

def on_find_in_files(cmd):
    """
    Find list of files with a given extension and from a given root directory    
    
    Example command:
    
    {"command":"ls", "file_extensions":[".cpp",".hpp",".h"], "root_dir":"/c/src/codelite", "find_what":"wxStringSet_t"}
    """
    files_arr = _get_list_of_files(cmd)
    find_what = cmd['find_what']
    return_obj = []
    for file in files_arr:
        lines = open(file=file, mode="r", encoding="utf-8").read().splitlines()
        line_number = 0
        entries = []
        for line in lines:
            line_number += 1
            matches = []
            for m in re.finditer(find_what, line):
                matches.append({
                    "start": m.start(),
                    "end": m.end()
                })
                
            if len(matches) > 0:
                for match in matches:
                    entries.append({
                        "ln": line_number, 
                        "start": match['start'],
                        "end": match['end']
                    })
                return_obj.append({
                    "file": file,
                    "matches": entries
                })
    print(json.dumps(return_obj))

def main_loop():
    """
    accept input from the user and process the command    
    """
    handlers = { 
        "ls": on_find_files, 
        "find": on_find_in_files,
        "exec": on_exec
    }
    while True:
        try:
            text = input("(codelite)")
            text = text.strip()
            if text == "exit" or text == "bye" or text == "quit":
                exit(0)
            
            # split the command line by spaces
            command = json.loads(text)
            func = handlers.get(command['command'], None)
            if func is not None:
                func(command)
            else:
                eprint("unknown command '{}'".format(command['command']))
        except Exception as e:
            eprint(e)

def main():
    main_loop()

if __name__ == "__main__":
    main()