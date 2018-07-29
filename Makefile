BIN_DIR        = bin
EXAMPLE_DIR    = examples

ORG_BASE_DIR   = org
ORG_CLASS_DIR  = $(ORG_BASE_DIR)/class
ORG_DRIVER_DIR = $(ORG_BASE_DIR)/driver

SRC_BASE_DIR   = src
SRC_CLASS_DIR  = $(SRC_BASE_DIR)/class
SRC_DRIVER_DIR  = $(SRC_BASE_DIR)/driver


EXEC=$(BIN_DIR)/run

BOOST_ROOT = /usr/local/opt/boost
BOOST_INCS = $(BOOST_ROOT)/include/boost/include
BOOST_LIBS = $(BOOST_ROOT)/lib/

CC = g++-7
CFLAGS += -g -std=c++11
LDFLAGS= -L$(BOOST_LIBS) -lboost_program_options

TANGLE_PREFIX = emacs --batch -l org  --no-init-file
TANGLE_SUFFIX = -f org-babel-tangle --kill

DEPS += -I$(BOOST_INCS)
DEPS += -I$(SRC_CLASS_DIR)

TEST_COMMAND_1 = $(EXEC) \
	-i examples/inputs/sine_sweep.raw \
	-o examples/outputs/sine_sweep_filt.raw

all: filt-main noise-main

run: filt-main
	$(TEST_COMMAND_1)

grind: main
	valgrind -v $(TEST_COMMAND_1)

filt-main: $(SRC_CLASS_DIR)/cb.h $(SRC_CLASS_DIR)/filt.h $(SRC_DRIVER_DIR)/filt-main.cc
	$(CC) $(CFLAGS) $(DEPS) $? -o $(EXEC) $(LDFLAGS)

noise-main: $(SRC_CLASS_DIR)/noise.h $(SRC_DRIVER_DIR)/noise-main.cc
	$(CC) $(CFLAGS) $(DEPS) $? -o $(BIN_DIR)/noise-test $(LDFLAGS)

$(SRC_DRIVER_DIR)/%-main.cc: $(ORG_CLASS_DIR)/spon-%.org
	emacs $< --batch -f org-babel-tangle --kill

$(SRC_CLASS_DIR)/%.h: $(ORG_CLASS_DIR)/spon-%.org
	emacs $< --batch -f org-babel-tangle --kill

clean:
	rm -f $(EXEC)
