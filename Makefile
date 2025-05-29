# =============================================================================
# OpenACCV-V: OpenACC Accelerator Validation & Verification Suite
# =============================================================================
#
# File:		Makefile
# Description: Build system for the OpenACC VV testsuite
#			 Supports compilation and execution of C/C++/Fortran tests
#
# Repository: https://github.com/OpenACCUserGroup/OpenACCV-V
#
# Usage:		make [COMPILER_OPTIONS] [BUILD_OPTIONS] [TARGET]
#			 See 'make help' for detailed options
# =============================================================================

# Define shell with better error handling
SHELL := /bin/bash -o pipefail

# When using make alone show the help message
.DEFAULT_GOAL := help

###################################################
# Configuration Variables
###################################################
# Test configuration
SYSTEM ?= generic
LOG_NOTE ?= "none"
LOGDIRNAME ?= logs
OMP_VERSION ?= 2.5

# System specific variables from system definition files
ifdef OMPVV_SYSTEM
	SYSTEM = ${OMPVV_SYSTEM}
else
	SYSTEM ?= generic
endif
-include sys/systems/$(SYSTEM).def

# Load compiler settings from make.def
# Set OMPVV_NO_COMPILER_MODULE_CHANGES to prevent changes
include sys/make/make.def

###################################################
# Logging Configuration
###################################################
# Quiet operation by default, enable with VERBOSE=1
QUIET := @
ifdef VERBOSE
	QUIET :=
endif

# Configure log file destinations
RECORD :=
LOGDIR :=
ifdef LOG
	LOGDIR := $(LOGDIRNAME)
	RECORD := | tee -a $(LOGDIR)/
endif
ifdef LOG_ALL
	LOG := 1	# LOG_ALL implies LOG
	LOGDIR := $(LOGDIRNAME)
	RECORD := 2>&1 | tee -a $(LOGDIR)/
endif

# Enable verbose mode for tests if requested
ifdef VERBOSE_TESTS
	VERBOSE_MODE = -DVERBOSE_MODE=1
endif

###################################################
# Batch Scheduler Configuration
###################################################
BSRUN :=
ifdef ADD_BATCH_SCHED
	BSRUN := $(BATCH_SCHEDULER)
endif

###################################################
# Test Tools and Report Configuration
###################################################
# Test running and results analyzer
RUN_TEST = $(CURDIR)/sys/scripts/run_test.sh
RESULTS_ANALYZER = $(CURDIR)/sys/scripts/createSummary.py
RESULTS_JSON_OUTPUT_FILE = results.json
RESULTS_CSV_OUTPUT_FILE = results.csv
RESULTS_HTML_OUTPUT_FOLDER = results_report
RESULTS_HTML_REPORT_TEMPLATE = $(CURDIR)/sys/results_template
REPORT_ONLINE_CONNECTION = $(CURDIR)/sys/scripts/onlineConnection.py

###################################################
# Source Files Configuration
###################################################
# Check for deprecated flags
ifneq "$(SOURCES_C)$(SOURCES_CPP)$(SOURCES_F)$(TESTS_TO_RUN)" ""
$(error The SOURCES_C SOURCES_CPP SOURCES_F and TESTS_TO_RUN flags were deprecated. Use SOURCES instead)
endif

# Process specified source files
ifneq "$(SOURCES)" ""
# Find source files matching the pattern
SOURCES_C := $(shell find $(CURDIR)/tests/ -path "*$(SOURCES)" | grep "\.c$$")
SOURCES_CPP := $(shell find $(CURDIR)/tests/ -path "*$(SOURCES)" | grep "\.cpp$$")
SOURCES_F := $(shell find $(CURDIR)/tests/ -path "*$(SOURCES)" | grep "\(\.F90\|\.F95\|\.F03\|\.F\|\.FOR\)$$")
$(info SOURCES = $(notdir $(SOURCES_C) $(SOURCES_CPP) $(SOURCES_F)))

# Find previously compiled tests matching these sources
TESTS_TO_RUN := $(foreach testName, \
								$(notdir $(SOURCES_C) $(SOURCES_CPP) $(SOURCES_F)), \
								$(shell test -d $(BINDIR) && find $(BINDIR) -name "$(testName)*"))
TESTS_TO_RUN := $(TESTS_TO_RUN:.FOR.o=.FOR.FOR.o) # Adding .FOR.o to fortran
TESTS_TO_RUN := $(TESTS_TO_RUN:.F90.o=.F90.FOR.o)
TESTS_TO_RUN := $(TESTS_TO_RUN:.F95.o=.F95.FOR.o)
TESTS_TO_RUN := $(TESTS_TO_RUN:.F03.o=.F03.FOR.o)
TESTS_TO_RUN := $(TESTS_TO_RUN:.F.o=.F.FOR.o)
RUN_TESTS = $(TESTS_TO_RUN:.o=.runonly)

# Define object files based on available compilers
ifneq "$(CC)" "none"
OBJS_C := $(SOURCES_C:.c=.c.o)
endif
ifneq "$(CXX)" "none"
OBJS_CPP := $(SOURCES_CPP:.cpp=.cpp.o)
endif
ifneq "$(FC)" "none"
OBJS_F := $(SOURCES_F:.FOR=.FOR.FOR.o)
OBJS_F := $(OBJS_F:.F90=.F90.FOR.o)
OBJS_F := $(OBJS_F:.F95=.F95.FOR.o)
OBJS_F := $(OBJS_F:.F03=.F03.FOR.o)
OBJS_F := $(OBJS_F:.F=.F.FOR.o)
endif
else
# No sources specified, build everything

# Find all source files in the project
ifdef LINK_OMPVV_LIB
SOURCES_C := $(shell find $(CURDIR)/tests/ -name "*.c")
else
SOURCES_C := $(shell find $(CURDIR)/tests/ ! -name qmcpack_target_static_lib.c -name "*.c")
endif
SOURCES_CPP := $(shell find $(CURDIR)/tests/ -name "*.cpp")
SOURCES_F := $(shell find $(CURDIR)/tests/ -name "*.F90" -o -name "*.F95" -o -name "*.F03" -o -name "*.F" -o -name "*.FOR" | grep -v "ompvv.F90")

# Find all previously compiled binaries
TESTS_TO_RUN := $(shell test -d $(BINDIR) && \
											find $(BINDIR) -name "*.F90.o" \
											-o -name "*.F95.o" \
											-o -name "*.F03.o" \
											-o -name "*.F.o" \
											-o -name "*.FOR.o" \
											-o -name "*.c.o" \
											-o -name "*.cpp.o")
TESTS_TO_RUN := $(TESTS_TO_RUN:.FOR.o=.FOR.FOR.o) # Adding .FOR.o to fortran
TESTS_TO_RUN := $(TESTS_TO_RUN:.F90.o=.F90.FOR.o)
TESTS_TO_RUN := $(TESTS_TO_RUN:.F95.o=.F95.FOR.o)
TESTS_TO_RUN := $(TESTS_TO_RUN:.F03.o=.F03.FOR.o)
TESTS_TO_RUN := $(TESTS_TO_RUN:.F.o=.F.FOR.o)
RUN_TESTS := $(TESTS_TO_RUN:.o=.runonly)

# Define object files based on available compilers
ifneq "$(CC)" "none"
OBJS_C := $(SOURCES_C:.c=.c.o)
endif
ifneq "$(CXX)" "none"
OBJS_CPP := $(SOURCES_CPP:.cpp=.cpp.o)
endif
ifneq "$(FC)" "none"
OBJS_F := $(SOURCES_F:.F90=.F90.FOR.o)
OBJS_F := $(OBJS_F:.F95=.F95.FOR.o)
OBJS_F := $(OBJS_F:.F03=.F03.FOR.o)
OBJS_F := $(OBJS_F:.F=.F.FOR.o)
OBJS_F := $(OBJS_F:.FOR=.FOR.FOR.o)
endif
endif

# Build dependency lists
COMP_DEP := $(OBJS_C) $(OBJS_CPP) $(OBJS_F)

# Get dependencies for the 'all' rule
ALL_DEP :=
ifneq "$(CC)" "none"
ALL_DEP += $(addprefix $(BINDIR)/,$(notdir $(SOURCES_C:.c=.c.run)))
endif
ifneq "$(CXX)" "none"
ALL_DEP += $(addprefix $(BINDIR)/,$(notdir $(SOURCES_CPP:.cpp=.cpp.run)))
endif
ifneq "$(FC)" "none"
ALL_DEP += $(addprefix $(BINDIR)/,$(notdir $(SOURCES_F:.F90=.F90.FOR.run)))
ALL_DEP := $(addprefix $(BINDIR)/,$(notdir $(ALL_DEP:.F95=.F95.FOR.run)))
ALL_DEP := $(addprefix $(BINDIR)/,$(notdir $(ALL_DEP:.F03=.F03.FOR.run)))
ALL_DEP := $(addprefix $(BINDIR)/,$(notdir $(ALL_DEP:.F=.F.FOR.run)))
ALL_DEP := $(addprefix $(BINDIR)/,$(notdir $(ALL_DEP:.FOR=.FOR.FOR.run)))
endif

# Set temp file for all targets
$(ALL_DEP) $(COMP_DEP) $(RUN_TESTS): LOGTEMPFILE := $(LOGTEMPFILE)

###################################################
# Offloading Configuration
###################################################
ifdef NO_OFFLOADING
	COFFLOADING = $(C_NO_OFFLOADING)
	CXXOFFLOADING = $(CXX_NO_OFFLOADING)
	FOFFLOADING = $(F_NO_OFFLOADING)
endif

###################################################
# Library Configuration
###################################################
OMPVVLIB = -L$(CURDIR)/include -lompvv
OMPVVLIB_DEP = $(CURDIR)/include/libompvv.a

###################################################
# Primary Build Targets
###################################################
.PHONY: all compile run message_display clean tidy

all: message_display $(ALL_DEP)
	@echo "==== COMPILE AND RUN DONE ===="

compile: message_display $(COMP_DEP)
	@echo "==== COMPILE DONE ===="

run: $(RUN_TESTS)
	@echo "==== RUN DONE ===="

message_display:
	@echo "==== OPENACCV-V TEST SUITE ===="
	@echo "Running with the following compilers:"
ifneq "$(CC)" "none"
	@echo "CC = $(CC) $(shell $(call loadModules,$(C_COMPILER_MODULE),"shut up") ${C_VERSION})"
endif
ifneq "$(CXX)" "none"
	@echo "CXX = $(CXX) $(shell $(call loadModules,$(CXX_COMPILER_MODULE),"shut up") ${CXX_VERSION})"
endif
ifneq "$(FC)" "none"
	@echo "FC = $(FC) $(shell $(call loadModules,$(F_COMPILER_MODULE),"shut up") ${F_VERSION})"
endif
	$(if $(MODULE_LOAD), @echo "C_MODULE = "$(subst ;,\;,$(C_COMPILER_MODULE)); echo "CXX_MODULE = "$(subst ;,\;,${CXX_COMPILER_MODULE}); echo "F_MODULE = "$(subst ;,\;,${F_COMPILER_MODULE}),)

###################################################
# Library Build Rules
###################################################
$(BINDIR)/libompvv.o: $(CURDIR)/include/libompvv.c $(BINDIR)
	@echo -e $(TXTYLW)"\n\n" compile: $< $(TXTNOC)
	$(call log_section_header,"COMPILE CC="${CCOMPILE},$(SYSTEM),$<,$(CC) $(shell $(call loadModules,$(C_COMPILER_MODULE),"shut up") $(C_VERSION)),$(OMP_VERSION),$(notdir $(@:.o=.log)))
	-$(QUIET)$(call loadModules,$(C_COMPILER_MODULE)) $(CCOMPILE) $(VERBOSE_MODE) $(DTHREADS) $(DTEAMS) $(HTHREADS) $< -c -o $(BINDIR)/$(notdir $@) $(if $(LOG),$(RECORD)$(notdir $(@:.o=.log))\
		&& echo "PASS" > $(LOGTEMPFILE) \
		|| echo "FAIL" > $(LOGTEMPFILE))
	-$(call log_section_footer,"COMPILE CC="${CCOMPILE},$(SYSTEM),$$(cat $(LOGTEMPFILE)),$(LOG_NOTE),$(notdir $(@:.o=.log)))
	-@$(if $(LOG), rm $(LOGTEMPFILE))

$(CURDIR)/include/libompvv.a: $(BINDIR)/libompvv.o
	ar -rc $(CURDIR)/include/libompvv.a $(BINDIR)/libompvv.o
	@ranlib $(CURDIR)/include/libompvv.a

###################################################
# Compilation Rules
###################################################
# C files compilation
%.c.o: %.c $(BINDIR) $(LOGDIR)
	@echo -e $(TXTYLW)"\n\n" compile: $< $(TXTNOC)
	$(call log_section_header,"COMPILE CC","$(SYSTEM)","$<","$(CC) $(shell $(call loadModules,$(C_COMPILER_MODULE),"shut up") $(C_VERSION))","$(OMP_VERSION)","$(notdir $(@:.o=.log))")
	-$(QUIET)$(call loadModules,$(C_COMPILER_MODULE)) $(CCOMPILE) $(VERBOSE_MODE) $(COMPILER_FLAGS) $< -o $(BINDIR)/$(notdir $@) $(if $(LOG),$(RECORD)$(notdir $(@:.o=.log)) && echo "PASS" > $(LOGTEMPFILE) || echo "FAIL" > $(LOGTEMPFILE))
	-$(call log_section_footer,"COMPILE CC","$(SYSTEM)","$$(cat $(LOGTEMPFILE))","$(LOG_NOTE)","$(notdir $(@:.o=.log))")
	-@$(if $(LOG), rm $(LOGTEMPFILE))

# Special rule for test that needs OMPVV lib
$(CURDIR)/tests/4.5/application_kernels/qmcpack_target_static_lib.c.o: $(CURDIR)/tests/4.5/application_kernels/qmcpack_target_static_lib.c $(BINDIR) $(LOGDIR) $(OMPVVLIB_DEP)
	@echo -e $(TXTYLW)"\n\n" compile: $< $(TXTNOC)
	$(call log_section_header,"COMPILE CC="${CCOMPILE},$(SYSTEM),$<,$(CC) $(shell $(call loadModules,$(C_COMPILER_MODULE),"shut up") $(C_VERSION)),$(OMP_VERSION),$(notdir $(@:.o=.log)))
	-$(QUIET)$(call loadModules,$(C_COMPILER_MODULE)) $(CCOMPILE) $(VERBOSE_MODE) $(DTHREADS) $(DTEAMS) $(HTHREADS) $< -o $(BINDIR)/$(notdir $@) $(OMPVVLIB) $(if $(LOG),$(RECORD)$(notdir $(@:.o=.log))\
		&& echo "PASS" > $(LOGTEMPFILE) \
		|| echo "FAIL" > $(LOGTEMPFILE))
	-$(call log_section_footer,"COMPILE CC="${CCOMPILE},$(SYSTEM),$$(cat $(LOGTEMPFILE)),$(LOG_NOTE),$(notdir $(@:.o=.log)))
	-@$(if $(LOG), rm $(LOGTEMPFILE))

# C++ files compilation
%.cpp.o: %.cpp $(BINDIR) $(LOGDIR)
	@echo -e $(TXTYLW)"\n\n" compile: $< $(TXTNOC)
	$(call log_section_header,"COMPILE CPP="${CXXCOMPILE},$(SYSTEM),$<,$(CXX) $(shell $(call loadModules,$(CXX_COMPILER_MODULE),"shut up") $(CXX_VERSION)),$(OMP_VERSION),$(notdir $(@:.o=.log)))
	-$(QUIET)$(call loadModules,$(CXX_COMPILER_MODULE)) $(CXXCOMPILE) $(VERBOSE_MODE) $(DTHREADS) $(DTEAMS) $(HTHREADS) $< -o $(BINDIR)/$(notdir $@) $(if $(LOG),$(RECORD)$(notdir $(@:.o=.log))\
		&& echo "PASS" > $(LOGTEMPFILE) \
		|| echo "FAIL" > $(LOGTEMPFILE))
	-$(call log_section_footer,"COMPILE","$(SYSTEM)","$$(cat $(LOGTEMPFILE))","$(LOG_NOTE)","$(notdir $(@:.o=.log))")
	-@$(if $(LOG), rm $(LOGTEMPFILE))

# Fortran files compilation
%.FOR.o: % $(BINDIR) $(LOGDIR) clear_fortran_mod
	@echo -e $(TXTYLW)"\n\n" compile: $< $(TXTNOC)
	$(call log_section_header,"COMPILE F="${FCOMPILE},$(SYSTEM),$<,$(FC) $(shell $(call loadModules,$(F_COMPILER_MODULE),"shut up") $(F_VERSION)),$(OMP_VERSION),$(notdir $(@:.FOR.o=.log)))
	-$(QUIET)$(call loadModules,$(F_COMPILER_MODULE)) $(FCOMPILE) $(VERBOSE_MODE) $(DTHREADS) $(DTEAMS) $(HTHREADS) $< -o $(BINDIR)/$(notdir $(@:.FOR.o=.o)) $(if $(LOG),$(RECORD)$(notdir $(@:.FOR.o=.log))\
		&& echo "PASS" > $(LOGTEMPFILE) \
		|| echo "FAIL" > $(LOGTEMPFILE))
	-$(call log_section_footer,"COMPILE","$(SYSTEM)","$$(cat $(LOGTEMPFILE))","$(LOG_NOTE)","$(notdir $(@:.FOR.o=.log))")
	-@$(if $(LOG), rm $(LOGTEMPFILE))

###################################################
# Test Execution Rules
###################################################
# C application execution
%.c.run: $(OBJS_C)
	$(call log_section_header,"RUN","$(SYSTEM)","$(@:.run=)","$(LOG_NOTE)","$(OMP_VERSION)","$(notdir $(@:.run=.log))")
	@echo -e $(TXTGRN)"\n\n" running: $@ $(TXTNOC) $(if $(LOG), ${RECORD}$(notdir $(@:.run=.log)))
	$(if $(findstring _env_,$@), \
		-$(call loadModules,$(C_COMPILER_MODULE)) \
		$(BSRUN)$(RUN_TEST) --env \
			$(shell echo "$@" | sed -e 's@.*/@@' -e 's@test_\(.*\)_env_.*@\1@' | tr 'a-z' 'A-Z') \
			$(shell echo "$@" | sed -e 's@.*/@@' -e 's@.*_env_\([^.]*\).*@\1@') \
			$(@:.run=.o) $(VERBOSE) $(if $(LOG),$(RECORD)$(notdir $(@:.run=.log)) \
		&& echo "PASS" > $(LOGTEMPFILE) \
		|| echo "FAIL" > $(LOGTEMPFILE)), \
		-$(call loadModules,$(C_COMPILER_MODULE)) $(BSRUN)$(RUN_TEST) $(@:.run=.o) $(VERBOSE) $(if $(LOG),$(RECORD)$(notdir $(@:.run=.log))\
		&& echo "PASS" > $(LOGTEMPFILE) \
		|| echo "FAIL" > $(LOGTEMPFILE)) \
	)
	-$(call log_section_footer,"RUN","$(SYSTEM)","$$(cat $(LOGTEMPFILE))","$(LOG_NOTE)","$(notdir $(@:.run=.log))")
	-@$(if $(LOG), rm $(LOGTEMPFILE))

# C++ application execution
%.cpp.run: $(OBJS_CPP)
	$(call log_section_header,"RUN","$(SYSTEM)","$(@:.run=)","$(LOG_NOTE)","$(OMP_VERSION)","$(notdir $(@:.run=.log))")
	@echo -e $(TXTGRN)"\n\n" running: $@ $(TXTNOC) $(if $(LOG), ${RECORD}$(notdir $(@:.run=.log)))
	-$(call loadModules,$(CXX_COMPILER_MODULE)) $(BSRUN)$(RUN_TEST) $(@:.run=.o) $(VERBOSE) $(if $(LOG),$(RECORD)$(notdir $(@:.run=.log))\
		&& echo "PASS" > $(LOGTEMPFILE) \
		|| echo "FAIL" > $(LOGTEMPFILE))
	-$(call log_section_footer,"RUN","$(SYSTEM)","$$(cat $(LOGTEMPFILE))","$(LOG_NOTE)","$(notdir $(@:.run=.log))")
	-@$(if $(LOG), rm $(LOGTEMPFILE))

# Fortran application execution
%.FOR.run: $(OBJS_F)
	$(call log_section_header,"RUN","$(SYSTEM)","$(@:.FOR.run=)","$(LOG_NOTE)","$(OMP_VERSION)","$(notdir $(@:.FOR.run=.log))")
	@echo -e $(TXTGRN)"\n\n" running: $@ $(TXTNOC) $(if $(LOG), ${RECORD}$(notdir $(@:.FOR.run=.log)))
	-$(call loadModules,$(F_COMPILER_MODULE)) $(BSRUN)$(RUN_TEST) $(@:.FOR.run=.o) $(VERBOSE) $(if $(LOG),$(RECORD)$(notdir $(@:.FOR.run=.log))\
		&& echo "PASS" > $(LOGTEMPFILE) \
		|| echo "FAIL" > $(LOGTEMPFILE))
	-$(call log_section_footer,"RUN","$(SYSTEM)","$$(cat $(LOGTEMPFILE))","$(LOG_NOTE)","$(notdir $(@:.FOR.run=.log))")
	-@$(if $(LOG), rm $(LOGTEMPFILE))

###################################################
# Run-Only Rules (No Compilation)
###################################################
# C application run-only
%.c.runonly:
	$(call log_section_header,"RUN","$(SYSTEM)","$(@:.runonly=)","$(LOG_NOTE)","$(OMP_VERSION)","$(notdir $(@:.runonly=.log))")
	@echo -e $(TXTGRN)"\n\n" running previously compiled: $@ $(TXTNOC) $(if $(LOG), ${RECORD}$(notdir $(@:.runonly=.log)))
	$(if $(findstring _env_,$@), \
		$(call loadModules,$(C_COMPILER_MODULE)) \
		$(BSRUN)$(RUN_TEST) --env \
			$(shell echo "$@" | sed -e 's@.*/@@' -e 's@test_\(.*\)_env_.*@\1@' | tr 'a-z' 'A-Z') \
			$(shell echo "$@" | sed -e 's@.*/@@' -e 's@.*_env_\([^.]*\).*@\1@') \
			$(@:.runonly=.o) $(VERBOSE) $(if $(LOG),$(RECORD)$(notdir $(@:.runonly=.log))\
		&& echo "PASS" > $(LOGTEMPFILE) \
		|| echo "FAIL" > $(LOGTEMPFILE)), \
		$(call loadModules,$(C_COMPILER_MODULE)) $(BSRUN)$(RUN_TEST) $(@:.runonly=.o) $(VERBOSE) $(if $(LOG),$(RECORD)$(notdir $(@:.runonly=.log))\
		&& echo "PASS" > $(LOGTEMPFILE) \
		|| echo "FAIL" > $(LOGTEMPFILE)) \
	)
	-$(call log_section_footer,"RUN","$(SYSTEM)","$$(cat $(LOGTEMPFILE))","$(LOG_NOTE)","$(notdir $(@:.runonly=.log))")
	-@$(if $(LOG), rm $(LOGTEMPFILE))

# C++ application run-only
%.cpp.runonly:
	$(call log_section_header,"RUN","$(SYSTEM)","$(@:.runonly=)","$(LOG_NOTE)","$(OMP_VERSION)","$(notdir $(@:.runonly=.log))")
	@echo -e $(TXTGRN)"\n\n" running previously compiled: $@ $(TXTNOC) $(if $(LOG), ${RECORD}$(notdir $(@:.runonly=.log)))
	-$(call loadModules,$(CXX_COMPILER_MODULE)) $(BSRUN)$(RUN_TEST) $(@:.runonly=.o) $(VERBOSE) $(if $(LOG),$(RECORD)$(notdir $(@:.runonly=.log))\
		&& echo "PASS" > $(LOGTEMPFILE) \
		|| echo "FAIL" > $(LOGTEMPFILE))
	-$(call log_section_footer,"RUN","$(SYSTEM)","$$(cat $(LOGTEMPFILE))","$(LOG_NOTE)","$(notdir $(@:.runonly=.log))")
	-@$(if $(LOG), rm $(LOGTEMPFILE))

# Fortran application run-only
%.FOR.runonly:
	$(call log_section_header,"RUN","$(SYSTEM)","$(@:.FOR.runonly=)","$(LOG_NOTE)","$(OMP_VERSION)","$(notdir $(@:.FOR.runonly=.log))")
	@echo -e $(TXTGRN)"\n\n" running previously compiled: $@ $(TXTNOC) $(if $(LOG), ${RECORD}$(notdir $(@:.FOR.runonly=.log)))
	-$(call loadModules,$(F_COMPILER_MODULE)) $(BSRUN)$(RUN_TEST) $(@:.FOR.runonly=.o) $(VERBOSE) $(if $(LOG),$(RECORD)$(notdir $(@:.FOR.runonly=.log))\
		&& echo "PASS" > $(LOGTEMPFILE) \
		|| echo "FAIL" > $(LOGTEMPFILE))
	-$(call log_section_footer,"RUN","$(SYSTEM)","$$(cat $(LOGTEMPFILE))","$(LOG_NOTE)","$(notdir $(@:.FOR.runonly=.log))")
	-@$(if $(LOG), rm $(LOGTEMPFILE))

###################################################
# Directory Creation Rules
###################################################
$(BINDIR):
	mkdir -p $@

$(LOGDIR):
	mkdir -p $@

###################################################
# Report Generation Rules
###################################################
.PHONY: report_csv report_json report_summary report_html report_online

$(RESULTS_CSV_OUTPUT_FILE):
	@echo "Creating $(RESULTS_CSV_OUTPUT_FILE) file"
	@echo "Currently we only support run logs that contain compilation and run outputs. Use the 'make all' rule to obtain these"
	@$(RESULTS_ANALYZER) -r -f csv -o $(RESULTS_CSV_OUTPUT_FILE) $(LOGDIRNAME)/*

$(RESULTS_JSON_OUTPUT_FILE):
	@echo "Creating $(RESULTS_JSON_OUTPUT_FILE) file"
	@echo "Currently we only support run logs that contain compilation and run outputs. Use the 'make all' rule to obtain these"
	@$(RESULTS_ANALYZER) -r -f json -o $(RESULTS_JSON_OUTPUT_FILE) $(LOGDIRNAME)/*

report_csv: $(RESULTS_CSV_OUTPUT_FILE)
	@echo "=== REPORT DONE ==="

report_json: $(RESULTS_JSON_OUTPUT_FILE)
	@echo "=== REPORT DONE ==="

report_summary:
	@$(RESULTS_ANALYZER) -r -f summary $(LOGDIRNAME)/*

report_html: $(RESULTS_JSON_OUTPUT_FILE) $(RESULTS_CSV_OUTPUT_FILE)
	@if [ -d "./$(RESULTS_HTML_OUTPUT_FOLDER)" ]; then \
		echo "A report already exists. Please move it before creating a new one"; \
	else \
		echo "=== CREATING REPORT ==="; \
		mkdir $(RESULTS_HTML_OUTPUT_FOLDER); \
		echo "Folder $(RESULTS_HTML_OUTPUT_FOLDER) created"; \
		cp -r $(RESULTS_HTML_REPORT_TEMPLATE)/* $(RESULTS_HTML_OUTPUT_FOLDER); \
		echo "Template copied"; \
		mv $(RESULTS_CSV_OUTPUT_FILE) $(RESULTS_HTML_OUTPUT_FOLDER); \
		mv $(RESULTS_JSON_OUTPUT_FILE) $(RESULTS_HTML_OUTPUT_FOLDER); \
		sed -i "1s/.*/var jsonResults = \[/g" $(RESULTS_HTML_OUTPUT_FOLDER)/$(RESULTS_JSON_OUTPUT_FILE); \
		sed -i "$$ s/.*/];/g" $(RESULTS_HTML_OUTPUT_FOLDER)/$(RESULTS_JSON_OUTPUT_FILE); \
		echo "JSON file processed"; \
	fi
	@echo "=== REPORT DONE ==="

# Validate configuration for online report
ifdef REPORT_ONLINE_TAG
	ifneq ("$(shell echo ${REPORT_ONLINE_TAG} | wc -m | grep -oh '[0-9]\+')", "10")
		$(error REPORT_ONLINE_TAG must be a 9-digit hexadecimal value)
	endif
	ifneq ("$(shell echo ${REPORT_ONLINE_TAG} | grep -E '^[a-f0-9]+$$')" , "")
		$(error REPORT_ONLINE_TAG must contain only hexadecimal digits (a-f, 0-9))
	endif
endif

ifdef REPORT_ONLINE_APPEND
	ifndef REPORT_ONLINE_TAG
		$(error To append to an online report, REPORT_ONLINE_TAG is required)
	endif
endif

report_online: $(RESULTS_JSON_OUTPUT_FILE)
	@echo "=== SUBMITTING ONLINE REPORT ===";
	@FLAGS=""; \
		if [ "${REPORT_ONLINE_TAG}" != "" ]; then \
			FLAGS="$$FLAGS -t ${REPORT_ONLINE_TAG}"; \
		fi; \
		if [ "1" == "${REPORT_ONLINE_APPEND}" ]; then \
			FLAGS="$$FLAGS -a"; \
		fi; \
		date >> recent_REPORT_ONLINE_tags; \
		${REPORT_ONLINE_CONNECTION} $$FLAGS ${RESULTS_JSON_OUTPUT_FILE} | tee -a recent_REPORT_ONLINE_tags;
	@echo "This tool is for visualization purposes."
	@echo "Our data retention policy is 1 month."
	@echo "After this time, we do not guarantee this link will work anymore"
	@echo "=== SUBMISSION DONE ==="

###################################################
# Cleanup Rules
###################################################
clean: clear_fortran_mod
	-rm -rf $(BINDIR)

clear_fortran_mod:
	-rm -f ./ompvv/*.mod

tidy: clean
	-rm -rf $(LOGDIRNAME)
	-rm -rf $(RESULTS_HTML_OUTPUT_FOLDER)
	-rm -rf $(RESULTS_JSON_OUTPUT_FILE)
	-rm -rf $(RESULTS_CSV_OUTPUT_FILE)
	-rm -rf ./*.exitstatus*
	-rm -rf *.mod
	-rm -rf slurm-*

###################################################
# Utility Rules
###################################################
.PHONY: compilers help

compilers:
	@echo "C compilers: $(CCOMPILERS)"
	@echo "C++ compilers: $(CXXCOMPILERS)"
	@echo "FORTRAN compilers: $(FCOMPILERS)"

help:
	@echo "OpenACC Validation & Verification Suite"
	@echo ""
	@echo "=== USAGE ==="
	@echo " make [COMPILER_OPTIONS] [BUILD_OPTIONS] [TARGET]"
	@echo ""
	@echo "=== COMPILER OPTIONS ==="
	@echo " CC=<c_compiler>		 C compiler to use (gcc, nvc, etc.)"
	@echo " CXX=<cpp_compiler>	 C++ compiler to use (g++, nvc++, etc.)"
	@echo " FC=<fortran_compiler>	Fortran compiler to use (gfortran, nvfortran, etc.)"
	@echo ""
	@echo "=== BUILD OPTIONS ==="
	@echo " VERBOSE=1				Show commands being executed"
	@echo " VERBOSE_TESTS=1		 Enable additional test output"
	@echo " LOG=1					Save build and test output to log files"
	@echo " LOG_ALL=1				Save stdout and stderr to log files"
	@echo " SYSTEM=<name>			Use system-specific configuration from sys/systems/<name>.def"
	@echo " NO_OFFLOADING=1		 Disable GPU offloading"
	@echo " SOURCES=<pattern>		Build/run only sources matching pattern"
	@echo ""
	@echo "=== TARGETS ==="
	@echo " all					 Build and run all tests (default)"
	@echo " compile				 Compile tests without running them"
	@echo " run					 Run previously compiled tests"
	@echo " clean					Remove compiled binaries"
	@echo " tidy					Remove all generated files (logs, reports, binaries)"
	@echo " report_csv			 Generate CSV report of test results"
	@echo " report_json			 Generate JSON report of test results"
	@echo " report_summary		 Generate summary of test results"
	@echo " report_html			 Generate HTML report of test results"
	@echo " compilers				Show available compiler configurations"