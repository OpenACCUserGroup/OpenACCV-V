# =============================================================================
# OpenACCV-V: OpenACC Accelerator Validation & Verification Suite
# =============================================================================
#
# Repository: https://github.com/OpenACCUserGroup/OpenACCV-V
# Usage:	 make [COMPILER OPTIONS] [BUILD OPTIONS] [TARGETS]
#			See 'make help' for detailed options
# =============================================================================

# Define shell with better error handling
SHELL := /bin/bash -o pipefail

# Default to help when no target is specified
.DEFAULT_GOAL := help

###################################################
# Configuration Variables
###################################################
# System configuration
SYSTEM	   ?= generic
LOG_NOTE	 ?= "none"
OACC_VERSION ?= 2.7

ifeq (,$(filter -j,$(MAKEFLAGS)))
SERIAL := 1
else
SERIAL := 0
endif

# Include system-specific definitions if available
-include src/systems/$(SYSTEM).def

# Load compiler settings and version detection
include src/make/make.def

###################################################
# Directory Configuration
###################################################
BUILDDIR = $(CURDIR)/build
BINDIR  = $(BUILDDIR)/bin
LOGDIR  = $(BUILDDIR)/logs
CONFIGDIR = $(BUILDDIR)/testsuite_config
TMPDIR = $(BUILDDIR)/tmp

###################################################
# Verbosity Configuration
###################################################
# Default to quiet operation, enable with VERBOSE=1
QUIET := @
ifdef VERBOSE
	QUIET :=
endif

###################################################
# Test Tools and Report Configuration
###################################################
RESULTS_JSON_OUTPUT_FILE = results.json

###################################################
# Source Files Configuration
###################################################
# Process specified source files
ifneq "$(SOURCES)" ""
	# Find source files matching the pattern
	SOURCES_C := $(shell find $(CURDIR)/tests/ -path "*$(SOURCES)" | grep "\.c$$")
	SOURCES_CPP := $(shell find $(CURDIR)/tests/ -path "*$(SOURCES)" | grep "\.cpp$$")
	SOURCES_F := $(shell find $(CURDIR)/tests/ -path "*$(SOURCES)" | grep "\.F90$$")
	$(info SOURCES = $(notdir $(SOURCES_C) $(SOURCES_CPP) $(SOURCES_F)))
else
	# No sources specified, find all source files
	SOURCES_C := $(shell find $(CURDIR)/tests/ -name "*.c")
	SOURCES_CPP := $(shell find $(CURDIR)/tests/ -name "*.cpp")
	SOURCES_F := $(shell find $(CURDIR)/tests/ -name "*.F90")
endif

# Find all the binary files that have been previously compiled
TESTS_TO_RUN := $(shell test -d $(BINDIR) && \
                        find $(BINDIR) -name "*.F90.o" \
                        -o -name "*.c.o" \
                        -o -name "*.cpp.o")
RUN_TESTS := $(TESTS_TO_RUN:.o=.runonly)

# Creating compile dependencies
ifneq "$(CC)" "none"
OBJS_C := $(patsubst %.c,$(BINDIR)/%.c.o,$(notdir $(SOURCES_C)))
endif
ifneq "$(CXX)" "none"
OBJS_CPP := $(patsubst %.cpp,$(BINDIR)/%.cpp.o,$(notdir $(SOURCES_CPP)))
endif
ifneq "$(FC)" "none"
OBJS_F := $(patsubst %.F90,$(BINDIR)/%.F90.o,$(notdir $(SOURCES_F)))
endif
COMP_DEP := $(OBJS_C) $(OBJS_CPP) $(OBJS_F)

# Get all the dependencies for all rule
ALL_DEP :=
ifneq "$(CC)" "none"
ALL_DEP += $(patsubst %.c,%.c.run,$(notdir $(SOURCES_C)))
endif
ifneq "$(CXX)" "none"
ALL_DEP += $(patsubst %.cpp,%.cpp.run,$(notdir $(SOURCES_CPP)))
endif
ifneq "$(FC)" "none"
ALL_DEP += $(patsubst %.F90,%.F90.run,$(notdir $(SOURCES_F)))
endif

###################################################
# Testsuite Configuration
###################################################

$(CONFIGDIR)/build_config.vars: | $(CONFIGDIR)
	@echo "SYSTEM=$(SYSTEM)" > $@
	@echo "LOG_NOTE=$(LOG_NOTE)" >> $@
	@echo "OACC_VERSION=$(OACC_VERSION)" >> $@
	@echo "COMPILER_VERSION=$(VERSION)" >> $@
	@echo "CC=$(CC)" >> $@
	@echo "CXX=$(CXX)" >> $@
	@echo "FC=$(FC)" >> $@
	@echo "CFLAGS=$(CFLAGS)" >> $@
	@echo "CXXFLAGS=$(CXXFLAGS)" >> $@
	@echo "FFLAGS=$(FFLAGS)" >> $@
	@echo "C_INCLUDES=$(C_INC)" >> $@
	@echo "CXX_INCLUDES=$(CXX_INC)" >> $@
	@echo "F_INCLUDES=$(F_INC)" >> $@
	@echo "VERBOSE=$(VERBOSE)" >> $@
	@echo "SERIAL=$(SERIAL)" >> $@
	@echo "PROJ_ROOT=$(CURDIR)" >> $@

###################################################
# Primary Build Targets
###################################################
.PHONY: all init compile run message_display $(COMPILE_TARGETS)

all : reset_counter reset_run_counter $(ALL_DEP)
	@echo ""
	@echo "==== COMPILE AND RUN DONE ===="

init : message_display 
	@python3 src/scripts/gen_config_json.py \
		--vars_file=$(CONFIGDIR)/build_config.vars \
		--output=$(CONFIGDIR)/build_config.json
	@echo "==== INITIALIZATION COMPLETE ===="

compile : reset_counter $(COMP_DEP) 
	@echo ""
	@echo "==== COMPILE DONE ===="

run : reset_run_counter $(ALL_DEP)
	@echo ""
	@echo "==== RUN DONE ===="

message_display: $(BINDIR) $(LOGDIR) $(TMPDIR) $(CONFIGDIR)/build_config.vars
	@echo "==== OPENACC V-V TESTSUITE ===="
	@echo "Running with the following compilers:"
ifneq "$(CC)" "none"
	@echo "CC = $(CC) $(C_VERSION)"
endif
ifneq "$(CXX)" "none"
	@echo "CXX = $(CXX) $(CXX_VERSION)"
endif
ifneq "$(FC)" "none"
	@echo "FC = $(FC) $(F_VERSION)"
endif

###################################################
# Compile Target
###################################################
TOTAL_FILES := $(words $(COMP_DEP))
COUNTER_FILE := $(TMPDIR)/.counter
LOCKFILE := $(TMPDIR)/.counter.lock

.PHONY: reset_counter
reset_counter: $(TMPDIR)
	@echo "0" > $(COUNTER_FILE)

# Pattern rules for compiling source files to object files in BINDIR
$(BINDIR)/%.c.o: %.c init $(BINDIR)
	@(flock -x 200; \
	COUNTER=$$(cat $(COUNTER_FILE)); \
	NEXT_COUNTER=$$((COUNTER+1)); \
	echo "$$NEXT_COUNTER" > $(COUNTER_FILE); \
	printf "\r[%3d/%-3d] Compiling %-50.50s" "$$NEXT_COUNTER" "$(TOTAL_FILES)" "$(notdir $<)"; \
	) 200>$(LOCKFILE); \
	python3 src/scripts/compile.py \
		--src="$<" 

$(BINDIR)/%.cpp.o: %.cpp init $(BINDIR)
	@(flock -x 200; \
	COUNTER=$$(cat $(COUNTER_FILE)); \
	NEXT_COUNTER=$$((COUNTER+1)); \
	echo "$$NEXT_COUNTER" > $(COUNTER_FILE); \
	printf "\r[%3d/%-3d] Compiling %-50.50s" "$$NEXT_COUNTER" "$(TOTAL_FILES)" "$(notdir $<)"; \
	) 200>$(LOCKFILE); \
	python3 src/scripts/compile.py \
		--src="$<" 

$(BINDIR)/%.F90.o: %.F90 init $(BINDIR)
	@(flock -x 200; \
	COUNTER=$$(cat $(COUNTER_FILE)); \
	NEXT_COUNTER=$$((COUNTER+1)); \
	echo "$$NEXT_COUNTER" > $(COUNTER_FILE); \
	printf "\r[%3d/%-3d] Compiling %-50.50s" "$$NEXT_COUNTER" "$(TOTAL_FILES)" "$(notdir $<)"; \
	) 200>$(LOCKFILE); \
	python3 src/scripts/compile.py \
		--src="$<" 

###################################################
# Run Target
###################################################
RUN_TOTAL_FILES := $(words $(ALL_DEP))
RUN_COUNTER_FILE := $(TMPDIR)/.run_counter
RUN_LOCKFILE := $(TMPDIR)/.run_counter.lock

.PHONY: reset_run_counter
reset_run_counter: $(TMPDIR)
	@echo "0" > $(RUN_COUNTER_FILE)

# Individual run targets with progress indicator
%.c.run: $(BINDIR)/%.c.o
	@(flock -x 201; \
	COUNTER=$$(cat $(RUN_COUNTER_FILE)); \
	NEXT_COUNTER=$$((COUNTER+1)); \
	echo "$$NEXT_COUNTER" > $(RUN_COUNTER_FILE); \
	printf "\r[%3d/%-3d] Running %-50.50s" "$$NEXT_COUNTER" "$(RUN_TOTAL_FILES)" "$(notdir $<)"; \
	) 201>$(RUN_LOCKFILE); \
	python3 src/scripts/run.py \
		--executable="$<" 

%.cpp.run: $(BINDIR)/%.cpp.o
	@(flock -x 201; \
	COUNTER=$$(cat $(RUN_COUNTER_FILE)); \
	NEXT_COUNTER=$$((COUNTER+1)); \
	echo "$$NEXT_COUNTER" > $(RUN_COUNTER_FILE); \
	printf "\r[%3d/%-3d] Running %-50.50s" "$$NEXT_COUNTER" "$(RUN_TOTAL_FILES)" "$(notdir $<)"; \
	) 201>$(RUN_LOCKFILE); \
	python3 src/scripts/run.py \
		--executable="$<" 

%.F90.run: $(BINDIR)/%.F90.o
	@(flock -x 201; \
	COUNTER=$$(cat $(RUN_COUNTER_FILE)); \
	NEXT_COUNTER=$$((COUNTER+1)); \
	echo "$$NEXT_COUNTER" > $(RUN_COUNTER_FILE); \
	printf "\r[%3d/%-3d] Running %-50.50s" "$$NEXT_COUNTER" "$(RUN_TOTAL_FILES)" "$(notdir $<)"; \
	) 201>$(RUN_LOCKFILE); \
	python3 src/scripts/run.py \
		--executable="$<" 

###################################################
# Directory Creation Rules
###################################################
$(BUILDDIR):
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

$(LOGDIR):
	mkdir -p $@

$(CONFIGDIR):
	mkdir -p $@

$(TMPDIR):
	mkdir -p $@

###################################################
# Report Generation Rules
###################################################
.PHONY: report_json report_summary

report_json: 
	@echo "Creating $(RESULTS_JSON_OUTPUT_FILE) file"
	@python3 src/scripts/run_report.py -l $(LOGDIR) -o $(RESULTS_JSON_OUTPUT_FILE)
	@echo "=== REPORT DONE ==="

report_summary:
	@$(RESULTS_ANALYZER) -r -f summary $(LOGDIRNAME)/*

###################################################
# Cleanup Rules
###################################################
clean: 
	-rm -rf $(BUILDDIR)

###################################################
# Utility Rules
###################################################
.PHONY: compilers help systems

compilers:
	@echo "=== AVAILABLE COMPILERS ==="
	@echo "Compilers: $(COMPILERS)"
	@echo "C compilers: $(CCOMPILERS)"
	@echo "C++ compilers: $(CXXCOMPILERS)"
	@echo "FORTRAN compilers: $(FCOMPILERS)"

systems:
	@echo "=== AVAILABLE SYSTEM CONFIGURATIONS ==="
	@echo "Current system: $(SYSTEM)"
	@echo "Available systems:"
	@find src/systems/ -name "*.def" | sed 's|src/systems/||g' | sed 's|.def||g' | sort | sed 's/^/  - /'

help:
	@echo "OpenACC Validation & Verification Suite"
	@echo ""
	@echo "=== USAGE ==="
	@echo " make [COMPILER OPTIONS] [BUILD OPTIONS] [TARGETS]"
	@echo ""
	@echo "=== COMPILER OPTIONS ==="
	@echo " COMPILER=<vendor>	 Select compiler vendor (nvidia, gnu, cray)"
	@echo " CC=<c_compiler>	   C compiler to use (gcc, nvc)"
	@echo " CXX=<cpp_compiler>	C++ compiler to use (g++, nvc++)"
	@echo " FC=<fortran_compiler> Fortran compiler to use (gfortran, nvfortran, ftn)"
	@echo ""
	@echo "=== BUILD OPTIONS ==="
	@echo " VERBOSE=1			 Show commands being executed"
	@echo " SYSTEM=<name>		 Use system-specific configuration from src/systems/<name>.def"
	@echo " SOURCES=<pattern>	 Build/run only sources matching pattern"
	@echo ""
	@echo "=== TARGETS ==="
	@echo " all				   Build and run all tests (default with specific target)"
	@echo " compile			   Compile tests without running them"
	@echo " run				   Run previously compiled tests"
	@echo " clean				 Remove build directory and temporary files"
	@echo " report_json		   Generate JSON report of test results"
	@echo " report_summary		Generate summary of test results"
	@echo " compilers			 Show available compiler configurations"
	@echo " systems			   Show available system configurations"
	@echo " help				  Show this help message"

###################################################
# Source File Search Paths
###################################################
# Help Make find source files when building object files in BINDIR
vpath %.c $(sort $(dir $(SOURCES_C)))
vpath %.cpp $(sort $(dir $(SOURCES_CPP)))
vpath %.F90 $(sort $(dir $(SOURCES_F)))

###################################################