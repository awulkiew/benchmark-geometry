#!/bin/bash

if [ "$#" -lt 1 ]; then
	echo 'Intended usage'
    echo '    run_clean SHA'
    exit
fi

rm -f results/$1
rm -Rf build
