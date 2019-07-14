#!/bin/sh

if [ ! -d bin/module ]
then
    mkdir bin/module
else
    unlink bin/sblog
    unlink bin/module/libsblog.so
fi

cp sylar/bin/sylar bin/sblog
cp lib/libsblog.so bin/module/
