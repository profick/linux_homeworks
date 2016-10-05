#!/bin/bash

#  If input is empty - use current directory.
#  Otherwise, use arguments as a directories.
[ $# -eq 0 ] && echo "You should input at least 1 directory" || directorys=$@

#  Function returns 0 (true) if last modification of argument was more than a week ago.
#  Returns 1 (false) otherwise.
isOld() {
    TMP=$(( $(date +%s) - $(stat -c %Y $1) ))
    [ $TMP -gt $SPW ] && return 0
    return 1
}

#  Function examine objects in argument's directory and all subdirectories.
#  If object is  
linkCheck () {
    for element in $1/*; do
        
        [ -h "$element" -a ! -e "$element" ] && isOld $element && echo \"$element\"
        [ -d "$element" ] && linkCheck $element
    done
}

#  Seconds Per Week:
SPW=$((60*60*24*7))

#  Checks all args[] directories.
for directory in $directorys; do
    if [ -d $directory ]
        then linkCheck $directory
        else
            echo "$directory is not a directory"
    fi
done

exit $?
