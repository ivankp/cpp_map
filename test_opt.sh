#!/bin/bash

for x in loop map
do
  echo $x
  g++ -std=c++20 -Iinclude -Wall -O3 \
    src/test/cmp_$x.cc \
  && objdump -d a.out | sed -n '/<main>/,${p;/^$/q}' \
  > $x.s
  rm -f a.out
done
