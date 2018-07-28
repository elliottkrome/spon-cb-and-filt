BDIR=bin
ODIR=$(BDIR)/obj
SDIR=src
EXAMPLE_DIR=examples
EXEC=$(BDIR)/run

CC = g++-7
CFLAGS += -g -std=c++11

BOOST_ROOT = /usr/local/opt/boost
BOOST_INCS = $(BOOST_ROOT)/include/boost/include
BOOST_LIBS = $(BOOST_ROOT)/lib/

HEADERS = $(wildcard $(SDIR)/*.h)
DEPS    = $(HEADERS)
DEPS += -I$(BOOST_INCS)

LDFLAGS= -L$(BOOST_LIBS) -lboost_program_options

TEST_COMMAND_1 = $(EXEC) \
	-i examples/inputs/sine_sweep.raw \
	-o examples/outputs/sine_sweep_filt.raw

# define a target for the application
all: main

run: main
	$(TEST_COMMAND_1)

main:
	$(CC) $(CFLAGS) $(DEPS) $(SDIR)/main.cc $^ -o $(EXEC)  $(LDFLAGS)

grind: main
	valgrind -v $(TEST_COMMAND_1) < $(EXAMPLE_DIR)/sine_sweep.raw > out

clean:
	rm -f $(EXEC)

