CC = clang

CONF_USE_FACILIO ?= 0

# Top-level directories.
SRC_DIR = src
BUILD_DIR = build
MODULES_DIR = modules

# Module directories.
JSON_C_DIR = $(MODULES_DIR)/json-c

# Utils build directories.
UTILS_BUILD_DIR = $(BUILD_DIR)/utils

# CWeb build directories.
CWEB_BUILD_DIR = $(BUILD_DIR)/cweb
CWEB_BUILD_DIR_OBJ = $(CWEB_BUILD_DIR)/obj

# Stress build directories.
STRESS_BUILD_DIR = $(BUILD_DIR)/cweb-stress
STRESS_BUILD_DIR_OBJ = $(STRESS_BUILD_DIR)/obj

# Utils source directory.
UTILS_SRC_DIR = $(SRC_DIR)/utils

# CWeb source directory.
CWEB_SRC_DIR = $(SRC_DIR)/cweb

# Utils helpers object.
UTILS_OBJ_HELPERS_SRC = helpers.c
UTILS_OBJ_HELPERS = helpers.o

# CWeb CLI object.
CWEB_OBJ_CLI_SRC = cli/cli.c
CWEB_OBJ_CLI = cli.o

# CWeb config object.
CWEB_OBJ_CONFIG_SRC = config/config.c
CWEB_OBJ_CONFIG = config.o

# CWeb logger object.
CWEB_OBJ_LOGGER_SRC = logger/logger.c
CWEB_OBJ_LOGGER = logger.o

# CWeb server object.
CWEB_OBJ_SERVER_SRC = server/server.c
CWEB_OBJ_SERVER = server.o

# CWeb server raw object.
CWEB_OBJ_SERVER_RAW_SRC = server/raw.c
CWEB_OBJ_SERVER_RAW = raw.o

# CWeb server Facilio object.
CWEB_OBJ_SERVER_FACILIO_SRC = server/facilio.c
CWEB_OBJ_SERVER_FACILIO = facilio.o

# CWeb HTTP objects.
CWEB_OBJ_HTTP_COMMON_SRC = http/common.c
CWEB_OBJ_HTTP_COMMON = http_common.o

CWEB_OBJ_HTTP_REQUEST_SRC = http/request.c
CWEB_OBJ_HTTP_REQUEST = http_request.o

CWEB_OBJ_HTTP_RESPONSE_SRC = http/response.c
CWEB_OBJ_HTTP_RESPONSE = http_response.o

# CWeb FS objects.
CWEB_OBJ_FS_WEB_SRC = fs/web.c
CWEB_OBJ_FS_WEB = fs_web.o

# CWeb objects (linker).
CWEB_OBJS = $(UTILS_BUILD_DIR)/$(UTILS_OBJ_HELPERS)
CWEB_OBJS += $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_CLI)
CWEB_OBJS += $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_CONFIG)
CWEB_OBJS += $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_LOGGER)
CWEB_OBJS += $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_SERVER) $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_SERVER_RAW) $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_SERVER_FACILIO)
CWEB_OBJS += $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_HTTP_COMMON) $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_HTTP_REQUEST) $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_HTTP_RESPONSE)
CWEB_OBJS += $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_FS_WEB)

# Common includes.
COMMON_INCS = -I $(SRC_DIR)

# CWeb main program.
CWEB_PROG_SRC = prog.c
CWEB_PROG_OUT = cweb

# CWeb-Strress source directory.
STRESS_SRC_DIR = $(SRC_DIR)/cweb-stress

# Stress CLI object.
STRESS_OBJ_CLI_SRC = cli/cli.c
STRESS_OBJ_CLI = cli.o

# Stress objects (linker).
STRESS_OBJS = $(UTILS_BUILD_DIR)/$(UTILS_OBJ_HELPERS) $(STRESS_BUILD_DIR_OBJ)/$(STRESS_OBJ_CLI)

STRESS_PROG_SRC = prog.c
STRESS_PROG_OUT = cweb-stress

# General flags.
FLAGS = -O2 -g

# Check for Facilio.
ifeq ($(CONF_USE_FACILIO), 1)
	FLAGS += -DCONF_USE_FACILIO
endif

all: setup utils_objs cweb_objs cweb_prog stress_objs stress_prog
setup:
	mkdir -p $(BUILD_DIR)

	mkdir -p $(UTILS_BUILD_DIR)

	mkdir -p $(CWEB_BUILD_DIR)
	mkdir -p $(CWEB_BUILD_DIR_OBJ)

	mkdir -p $(STRESS_BUILD_DIR)
	mkdir -p $(STRESS_BUILD_DIR_OBJ)
utils_objs: setup utils_obj_helpers
utils_obj_helpers:
	$(CC) $(FLAGS) -c $(COMMON_INCS) -o $(UTILS_BUILD_DIR)/$(UTILS_OBJ_HELPERS) $(UTILS_SRC_DIR)/$(UTILS_OBJ_HELPERS_SRC)
cweb_objs: setup cweb_obj_cli cweb_obj_config cweb_obj_logger cweb_obj_server cweb_obj_server_raw cweb_obj_server_facilio cweb_obj_http_common cweb_obj_http_request cweb_obj_http_response cweb_obj_fs_web
cweb_obj_cli:
	$(CC) $(FLAGS) -c $(COMMON_INCS) -o $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_CLI) $(CWEB_SRC_DIR)/$(CWEB_OBJ_CLI_SRC)
cweb_obj_config:
	$(CC) $(FLAGS) -c $(COMMON_INCS) -o $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_CONFIG) $(CWEB_SRC_DIR)/$(CWEB_OBJ_CONFIG_SRC)
cweb_obj_logger:
	$(CC) $(FLAGS) -c $(COMMON_INCS) -o $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_LOGGER) $(CWEB_SRC_DIR)/$(CWEB_OBJ_LOGGER_SRC)
cweb_obj_server:
	$(CC) $(FLAGS) -c $(COMMON_INCS) -o $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_SERVER) $(CWEB_SRC_DIR)/$(CWEB_OBJ_SERVER_SRC)
cweb_obj_server_raw:
	$(CC) $(FLAGS) -c $(COMMON_INCS) -o $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_SERVER_RAW) $(CWEB_SRC_DIR)/$(CWEB_OBJ_SERVER_RAW_SRC)
cweb_obj_server_facilio:
	$(CC) $(FLAGS) -c $(COMMON_INCS) -o $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_SERVER_FACILIO) $(CWEB_SRC_DIR)/$(CWEB_OBJ_SERVER_FACILIO_SRC)
cweb_obj_http_common:
	$(CC) $(FLAGS) -c $(COMMON_INCS) -o $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_HTTP_COMMON) $(CWEB_SRC_DIR)/$(CWEB_OBJ_HTTP_COMMON_SRC)
cweb_obj_http_request:
	$(CC) $(FLAGS) -c $(COMMON_INCS) -o $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_HTTP_REQUEST) $(CWEB_SRC_DIR)/$(CWEB_OBJ_HTTP_REQUEST_SRC)
cweb_obj_http_response:
	$(CC) $(FLAGS) -c $(COMMON_INCS) -o $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_HTTP_RESPONSE) $(CWEB_SRC_DIR)/$(CWEB_OBJ_HTTP_RESPONSE_SRC)
cweb_obj_fs_web:
	$(CC) $(FLAGS) -c $(COMMON_INCS) -o $(CWEB_BUILD_DIR_OBJ)/$(CWEB_OBJ_FS_WEB) $(CWEB_SRC_DIR)/$(CWEB_OBJ_FS_WEB_SRC)
cweb_prog: setup cweb_objs
	$(CC) $(FLAGS) $(COMMON_INCS) -ljson-c $(CWEB_OBJS) -o $(CWEB_BUILD_DIR)/$(CWEB_PROG_OUT) $(CWEB_SRC_DIR)/$(CWEB_PROG_SRC)
stress_objs: setup stress_obj_cli
stress_obj_cli:
	$(CC) $(FLAGS) -c $(COMMON_INCS) -o $(STRESS_BUILD_DIR_OBJ)/$(STRESS_OBJ_CLI) $(STRESS_SRC_DIR)/$(STRESS_OBJ_CLI_SRC)
stress_prog: setup stress_objs
	$(CC) $(FLAGS) $(COMMON_INCS) $(STRESS_OBJS) -o $(STRESS_BUILD_DIR)/$(STRESS_PROG_OUT) $(STRESS_SRC_DIR)/$(STRESS_PROG_SRC)
json-c:
	rm -rf $(JSON_C_DIR)/build
	cd $(JSON_C_DIR) && mkdir build && cd build && cmake ../
	$(MAKE) -C $(JSON_C_DIR)/build
json-c-install:
	$(MAKE) -C $(JSON_C_DIR)/build install
install:
	cp -f $(CWEB_BUILD_DIR)/$(CWEB_PROG_OUT) /usr/bin
	cp -f $(STRESS_BUILD_DIR)/$(STRESS_PROG_OUT) /usr/bin
clean:
	rm -f $(UTILS_BUILD_DIR)/*.o

	rm -f $(CWEB_BUILD_DIR)/cweb
	rm -f $(CWEB_BUILD_DIR_OBJ)/*.o

	rm -f $(STRESS_BUILD_DIR)/cweb-stress
	rm -f $(STRESS_BUILD_DIR_OBJ)/*.o
.PHONY: setup