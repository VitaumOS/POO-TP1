# Makefile at POO-TP1/
# ===================
.PHONY: build-and-run build run


dir_src				:= src/
dir_hdr				:= src/headers/
dir_bin				:= bin/

source_files		:= $(wildcard $(dir_src)*.cpp)
header_files		:= $(wildcard $(dir_hdr)*.h)

executable			:= $(dir_bin)workshop.exe

COMPILE_DIRECTIVES	:= -Wall -Wextra -O1 -I$(dir_hdr)


## 

build-and-run: build run

build: $(executable)
run:
	./$(executable)


$(executable): $(source_files) $(header_files)
	g++ $(COMPILE_DIRECTIVES) $(source_files) -o $@
