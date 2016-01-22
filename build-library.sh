#!/bin/bash
gcc -fPIC --shared -o libprimegen.so primegen_init.c primegen_next.c primegen.c primegen_skip.c

