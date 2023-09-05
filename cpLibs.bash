#!/bin/sh

lib_items=(
    "Arduino_OBD2.hpp"
    "Arduino_OBD2.cpp"
    "OBD2_command.hpp"
    "OBD2_command.cpp"
    "PID_LIST.h"
)

for dir in `find $(dirname $0) -type f -name *.ino` ;do
    for i in "${lib_items[@]}"; do
        cp -f $(dirname $0)/${i} $(dirname $dir)/${i}
    done
done