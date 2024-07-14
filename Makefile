# Makefile at <POO-TP1/>
# =======================
# Last update: 14/07/2024.
.PHONY: build-and-run 
.PHONY: build run
.PHONY: clean full-clean erase-data 


# Directories
# -----------
# * All relative to the working directory this file.

dir_src				:= src/
dir_hdr				:= src/headers/
dir_bin				:= bin/
dir_obj				:= $(dir_bin)obj/
dir_dat				:= data/			
dir_tmp				:= $(dir_dat)cache/

# Files
# -----

source_files		:= $(wildcard $(dir_src)*.cpp)
header_files		:= $(wildcard $(dir_hdr)*.h)
object_files		:= $(foreach object,$(subst .cpp,.o,$(subst $(dir_src),,$(source_files))),$(dir_obj)$(object))

target_exe			:= $(dir_bin)workshop.exe

dummy_cache_file	:= $(dir_tmp)__nothing__.log


# Building
# ========

EXE_COMPILE_DIRECTIVES		:= -Wall -Wextra -O3 -I"$(dir_hdr)"
OBJ_COMPILE_DIRECTIVES		:= -Wall -Wextra -O3 -I"$(dir_hdr)"

# Building the path...
ifeq ($(OS),Windows_NT)
define MAKEDIR =
	if not exist "$(@D)" mkdir "$(@D)"
endef
else
define MAKEDIR
	mkdir -p "$(@D)"
endef
endif


build-and-run: build run


build: $(target_exe)
	@echo System assembled.

$(target_exe): $(object_files)
	@$(MAKEDIR)	
	@g++ $(COMPILE_DIRECTIVES) $^ -o $@

$(dir_obj)%.o: $(dir_src)%.cpp $(header_files)
	@$(MAKEDIR)
	@g++ -c $< -o $@ $(OBJ_COMPILE_DIRECTIVES)
	@echo - Compiled $@


__build_dependencies_dir: $(dummy_cache_file)
	
$(dummy_cache_file):
	@$(MAKEDIR)


# Cleaning
# ========

# Deletes automatically the path specified by the 
# firstmost dependency.
ifeq ($(OS),Windows_NT)
    CMD_remove = if exist "$<" rmdir /s /q "$<"
else
    CMD_remove = rm -rf "$<"
endif


full-clean: __clean_bin
	@echo All program binaries cleaned.

clean: __clean_obj
	@echo Byproduct binaries cleaned.

__clean_bin: $(dir_bin)
	@$(CMD_remove)

__clean_obj: $(dir_obj)
	@$(CMD_remove)

erase-data: $(dir_dat) __build_dependencies_dir
	@$(CMD_remove)


$(dir_bin):
$(dir_obj):
$(dir_obj):


# Running
# =======

run:
	@echo - Running: $(target_exe)	
	@./$(target_exe)


.PHONY: __build_dependencies_dir
.PHONY: __clean_bin __clean_data 