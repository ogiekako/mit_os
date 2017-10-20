#!/bin/bash

n=$1
git clone https://pdos.csail.mit.edu/6.828/2017/jos.git lab$n
cd lab$n
git co -b lab$n origin/lab$n
rm -rf .git
