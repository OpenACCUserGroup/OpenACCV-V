# Compiler definitions
CC = nvc
CXX = nvc++
FC = nvfortran

# Directories
SRC_DIR = Tests
BIN_DIR = bin

# Source files
C_SRCS := $(wildcard $(SRC_DIR)/*.c)
CPP_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
F_SRCS := $(wildcard $(SRC_DIR)/*.f90)

# Output binaries
C_BINS := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.c.out, $(C_SRCS))
CPP_BINS := $(patsubst $(SRC_DIR)/%.cpp, $(BIN_DIR)/%.cpp.out, $(CPP_SRCS))
F_BINS := $(patsubst $(SRC_DIR)/%.f90, $(BIN_DIR)/%.f90.out, $(F_SRCS))

# Compiler flags
ifdef VERBOSE
    ACCFLAGS = -acc=gpu -Minfo=all
else
    ACCFLAGS = -acc=gpu
endif

# Handle single test
ifneq ($(TEST),)
.PHONY: test
test: prepare
	@ext=$$(echo "$(TEST)" | rev | cut -d. -f1 | rev); \
	name=$$(basename "$(TEST)"); \
	base=$${name%.*}; \
	out="$(BIN_DIR)/$$name.out"; \
	if [ "$$ext" = "h" ]; then \
		echo "Header file detected. Nothing to build."; \
		exit 0; \
	elif [ "$$ext" = "c" ]; then \
		echo "$(CC) $(ACCFLAGS) -o $$out $(TEST)"; \
		$(CC) $(ACCFLAGS) -I$(SRC_DIR) -o $$out $(TEST); \
	elif [ "$$ext" = "cpp" ]; then \
		echo "$(CXX) $(ACCFLAGS) -o $$out $(TEST)"; \
		$(CXX) $(ACCFLAGS) -I$(SRC_DIR) -o $$out $(TEST); \
	elif [ "$$ext" = "f90" ]; then \
		echo "$(FC) $(ACCFLAGS) -o $$out $(TEST)"; \
		$(FC) $(ACCFLAGS) -I$(SRC_DIR) -o $$out $(TEST); \
	else \
		echo "Unsupported file type: $$ext"; \
		exit 1; \
	fi; \
	echo "Running $$out"; \
	./$$out; \
	status=$$?; \
	if [ $$status -eq 0 ]; then echo "PASS"; else echo "FAIL (code $$status)"; fi

.DEFAULT_GOAL := test
else

.DEFAULT_GOAL := all

# Default target builds all
.PHONY: all
all: prepare c cpp fortran

# Compile and run C tests
.PHONY: c
c: $(C_BINS)
	@for bin in $^; do \
		echo "Running $$bin"; \
		./$$bin; \
		status=$$?; \
		if [ $$status -eq 0 ]; then echo "PASS"; else echo "FAIL (code $$status)"; fi; \
	done

# Compile and run C++ tests
.PHONY: cpp
cpp: $(CPP_BINS)
	@for bin in $^; do \
		echo "Running $$bin"; \
		./$$bin; \
		status=$$?; \
		if [ $$status -eq 0 ]; then echo "PASS"; else echo "FAIL (code $$status)"; fi; \
	done

# Compile and run Fortran tests
.PHONY: fortran
fortran: $(F_BINS)
	@for bin in $^; do \
		echo "Running $$bin"; \
		./$$bin; \
		status=$$?; \
		if [ $$status -eq 0 ]; then echo "PASS"; else echo "FAIL (code $$status)"; fi; \
	done

# Compilation rules
$(BIN_DIR)/%.c.out: $(SRC_DIR)/%.c
	@echo "$(CC) $(ACCFLAGS) -o $@ $<"
	$(CC) $(ACCFLAGS) -I$(SRC_DIR) -o $@ $<

$(BIN_DIR)/%.cpp.out: $(SRC_DIR)/%.cpp
	@echo "$(CXX) $(ACCFLAGS) -o $@ $<"
	$(CXX) $(ACCFLAGS) -I$(SRC_DIR) -o $@ $<

$(BIN_DIR)/%.f90.out: $(SRC_DIR)/%.f90
	@echo "$(FC) $(ACCFLAGS) -o $@ $<"
	$(FC) $(ACCFLAGS) -I$(SRC_DIR) -o $@ $<

endif

# Create bin directory
.PHONY: prepare
prepare:
	@mkdir -p $(BIN_DIR)

# Clean build artifacts
.PHONY: clean
clean:
	@rm -rf $(BIN_DIR)

