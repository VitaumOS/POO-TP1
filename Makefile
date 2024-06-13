# Makefile at POO-TP1/
# 
# TODO: build, clean, run.
#
#


dir_src			:= src/
dir_hdr			:= src/headers/
dir_bin			:= bin/


source_files	:= $(wildcard $(dir_src)*.cpp)
header_files	:= $(wildcard $(dir_src)*.h)

executable		:= $(dir_bin)workshop.exe

COMPILE_DIRECTIVES	:= -Wall -Wextra -O3 -I$(dir_hdr)

build: $(executable)


$(executable): $(source_files) 
	g++ $(COMPILE_DIRECTIVES) $^ -o $@

$(source_files): $(header_files)
$(header_files):


.PHONY: build
