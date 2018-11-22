#!/bin/bash

git status -s|grep -v "??"|grep "\.svg"| awk '{print $2;}'|xargs ./convert-single-file.sh 
