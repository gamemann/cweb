CC = clang

# Top-level directories.
SRC_DIR = src
BUILD_DIR = build
MODULES_DIR = modules

# Module directories.
JSON_C_DIR = $(MODULES_DIR)/json-c

# Utils build directories.
UTILS_BUILD_DIR := $(BUILD_DIR)/utils
UTILS_BUILD_DIR_OBJ := $(UTILS_BUILD_DIR)/obj

# Utils source.
UTILS_SRC_DIR = $(SRC_DIR)/utils

UTILS_SRC_FILES := $(wildcard $(UTILS_SRC_DIR)/**/*.c)

# Utils objects.
UTILS_OBJS := $(patsubst $(UTILS_SRC_DIR)/%.c, $(UTILS_BUILD_DIR_OBJ)/%.o, $(UTILS_SRC_FILES))

# CWeb build directories.
CWEB_BUILD_DIR = $(BUILD_DIR)/cweb
CWEB_BUILD_DIR_OBJ = $(CWEB_BUILD_DIR)/obj

# CWeb source.
CWEB_SRC_DIR = $(SRC_DIR)/cweb

CWEB_PROG_SRC = prog.c
CWEB_PROG_OUT = cweb

CWEB_SRC_FILES := $(wildcard $(CWEB_SRC_DIR)/**/*.c)
CWEB_SRC_FILES := $(filter-out $(CWEB_SRC_DIR)/$(CWEB_PROG_SRC), $(CWEB_SRC_FILES))

# CWeb objects.
CWEB_OBJS := $(patsubst $(CWEB_SRC_DIR)/%.c, $(CWEB_BUILD_DIR_OBJ)/%.o, $(CWEB_SRC_FILES))
CWEB_OBJS += $(UTILS_OBJS)

# Stress build directories.
STRESS_BUILD_DIR = $(BUILD_DIR)/cweb-stress
STRESS_BUILD_DIR_OBJ = $(STRESS_BUILD_DIR)/obj

# CWeb-stres source.
STRESS_SRC_DIR  := $(SRC_DIR)/cweb-stress

STRESS_PROG_SRC := prog.c
STRESS_PROG_OUT := cweb-stress

STRESS_SRC_FILES := $(wildcard $(STRESS_SRC_DIR)/**/*.c)
STRESS_SRC_FILES := $(filter-out $(STRESS_SRC_DIR)/$(STRESS_PROG_SRC), $(STRESS_SRC_FILES))

# Stress objects (linker).
STRESS_OBJS := $(patsubst $(STRESS_SRC_DIR)/%.c, $(STRESS_BUILD_DIR_OBJ)/%.o, $(STRESS_SRC_FILES))
STRESS_OBJS += $(UTILS_OBJS)

# Common includes.
COMMON_INCS = -I $(SRC_DIR)

# General flags.
FLAGS = -O2 -g

DEP_FLAGS = -MMD -MP

all: cweb_prog stress_prog

$(UTILS_BUILD_DIR_OBJ)/%.o: $(UTILS_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) $(DEP_FLAGS) -c $(COMMON_INCS) -o $@ $<

$(CWEB_BUILD_DIR_OBJ)/%.o: $(CWEB_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) $(DEP_FLAGS) -c $(COMMON_INCS) -o $@ $<

cweb_prog: $(UTILS_OBJS) $(CWEB_OBJS)
	$(CC) $(FLAGS) $(COMMON_INCS) -lpthread -ljson-c $(CWEB_OBJS) -o $(CWEB_BUILD_DIR)/$(CWEB_PROG_OUT) $(CWEB_SRC_DIR)/$(CWEB_PROG_SRC)

$(STRESS_BUILD_DIR_OBJ)/%.o: $(STRESS_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) $(DEP_FLAGS) -c $(COMMON_INCS) -o $@ $<

stress_prog: $(UTILS_OBJS) $(STRESS_OBJS)
	$(CC) $(FLAGS) $(COMMON_INCS) $(STRESS_OBJS) -o $(STRESS_BUILD_DIR)/$(STRESS_PROG_OUT) $(STRESS_SRC_DIR)/$(STRESS_PROG_SRC)

json-c:
	rm -rf $(JSON_C_DIR)/build
	cd $(JSON_C_DIR) && mkdir build && cd build && cmake ../
	$(MAKE) -C $(JSON_C_DIR)/build
json-c-install:
	$(MAKE) -C $(JSON_C_DIR)/build install
json-c-clean:
	rm -rf $(JSON_C_DIR)/build

install:
	cp -f $(CWEB_BUILD_DIR)/$(CWEB_PROG_OUT) /usr/bin
	cp -f $(STRESS_BUILD_DIR)/$(STRESS_PROG_OUT) /usr/bin

clean:
	find $(UTILS_BUILD_DIR) -name '*.o' -delete
	find $(CWEB_BUILD_DIR_OBJ) -name '*.o' -delete
	find $(STRESS_BUILD_DIR_OBJ) -name '*.o' -delete

	rm -f $(CWEB_BUILD_DIR)/cweb
	rm -f $(STRESS_BUILD_DIR)/cweb-stress
clean-deep:
	rm -rf $(BUILD_DIR)

.PHONY: all clean clean-deep install json-c json-c-install json-c-clean

-include $(UTILS_OBJS:.o=.d) $(CWEB_OBJS:.o=.d) $(STRESS_OBJS:.o=.d)