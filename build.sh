#!/usr/bin/env bash
gcc -o javascript.so javascript.c $(yed --print-cflags) $(yed --print-ldflags)
