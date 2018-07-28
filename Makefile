BDIR=bin
SDIR=src
EXAMPLE_DIR=examples
EXEC=$(BDIR)/run

BOOST_ROOT = /usr/local/opt/boost
BOOST_INCS = $(BOOST_ROOT)/include/boost/include
BOOST_LIBS = $(BOOST_ROOT)/lib/

CC = g++-7
CFLAGS += -g -std=c++11
LDFLAGS= -L$(BOOST_LIBS) -lboost_program_options

HEADERS = $(wildcard $(SDIR)/*.h)
DEPS    = $(HEADERS)
DEPS += -I$(BOOST_INCS)

TEST_COMMAND_1 = $(EXEC) \
	-i examples/inputs/sine_sweep.raw \
	-o examples/outputs/sine_sweep_filt.raw

all: main

run: main
	$(TEST_COMMAND_1)

grind: main
	valgrind -v $(TEST_COMMAND_1)

main:
	$(CC) $(CFLAGS) $(DEPS) $(SDIR)/main.cc $^ -o $(EXEC)  $(LDFLAGS)

clean:
	rm -f $(EXEC)
