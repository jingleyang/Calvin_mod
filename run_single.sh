#!/bin/sh

env LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH  bin/deployment/db 1 m 0
