#!/bin/bash

for x in loop map
do
  printf $x
  time g++ -std=c++20 -Iinclude -Wall -O2 \
    src/test/cmp_$x.cc \
  && objdump -d a.out | sed -n '/<main>/,${p;/^$/q}' \
  > $x.s
  rm -f a.out
  echo
done

diff loop.s map.s

