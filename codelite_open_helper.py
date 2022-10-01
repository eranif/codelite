#!/usr/bin/python3
#
# Helper to open files in codelite, so we don't need to fire up an expensive codelite process
#
#

import errno
import json
import os
import socket
import subprocess
import sys

if len(sys.argv) == 1:
    print("usage: codelite_open_helper <filename> ...")
    exit(1)

filenames_to_open = sys.argv[1:]
# Convert to absolute paths, so codelite can open it, since codelite is probably running with a different
# root path.
for i in range(0, len(filenames_to_open)):
    filenames_to_open[i] = os.path.abspath(filenames_to_open[i])

# Try opening a socket to the codelite process
try:
    codelite_port = (os.getuid() % 57) + 13617
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('127.0.0.1', codelite_port))
    body = json.dumps({ "args" : filenames_to_open })
    header = str(len(body)).zfill(10)
    s.send((header + body).encode('utf-8'))
    s.close()
    exit(0) # Success!
except socket.error as serr:
    if serr.errno != errno.ECONNREFUSED:
        # Not the error we are looking for, re-raise
        raise serr

# OK, so we couldn't talk to codelite, let us see if it is running
pidof_subprocess = subprocess.Popen('pidof codelite', stdout=subprocess.PIPE, shell=True)
found_codelite = False
with pidof_subprocess.stdout as txt:
    for line in txt:
        line = line.strip()
        if len(line) != 0:
            found_codelite = True
pidof_subprocess.terminate()
if found_codelite:
    print("Found running codelite process, but could not talk to it, no idea what is wrong")
    exit(1)

# Since we didn't find it, let's just fire up codelite and open the files
filenames_to_open.insert(0, 'codelite')
subprocess.Popen(filenames_to_open)
