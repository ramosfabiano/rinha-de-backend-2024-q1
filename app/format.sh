#!/bin/bash

find . -name *.cc -exec clang-format -i --style="{BasedOnStyle: google, IndentWidth: 4, TabWidth: 4, ColumnLimit: 150}" {} \;
find . -name *.h  -exec clang-format -i --style="{BasedOnStyle: google, IndentWidth: 4, TabWidth: 4, ColumnLimit: 150}" {} \;

