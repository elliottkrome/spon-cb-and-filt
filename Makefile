BIN_DIR        = bin
EXAMPLE_DIR    = examples

ORG_BASE_DIR   = org
ORG_CLASS_DIR  = $(ORG_BASE_DIR)/class
ORG_DRIVER_DIR = $(ORG_BASE_DIR)/driver

SRC_BASE_DIR   = src
SRC_CLASS_DIR  = $(SRC_BASE_DIR)/class
SRC_DRIVER_DIR = $(SRC_BASE_DIR)/driver

THIRD_PARTY    = third-party
THIRD_PARTY_INCLUDE = $(THIRD_PARTY)/include
THIRD_PARTY_LIB = $(THIRD_PARTY)/lib

EXEC=$(BIN_DIR)/run

BOOST_ROOT = /usr/local/opt/boost
BOOST_INCS = $(BOOST_ROOT)/include/boost/include
BOOST_LIBS = $(BOOST_ROOT)/lib/

CC = g++-7
CFLAGS += -g -std=c++11
LDFLAGS += -L$(BOOST_LIBS) -lboost_program_options
LDFLAGS += -L$(THIRD_PARTY_LIB) -ltinyxml2

SPON_BASE_DIR_BATCH_MODE = --eval "(setq spon-filt-base-dir (locate-dominating-file buffer-file-name \".dir-locals.el\"))"
TANGLE_BATCH_SUFFIX = --batch -l org $(SPON_BASE_DIR_BATCH_MODE) -f org-babel-tangle --kill

DEPS += -I$(BOOST_INCS)
DEPS += -I$(SRC_CLASS_DIR)
DEPS += -I$(THIRD_PARTY_INCLUDE)

TEST_COMMAND_1 = $(EXEC) \
	-c examples/filters/low-pass-00.xml \
	-i examples/inputs/sine_sweep.raw \
	-o examples/outputs/sine_sweep_filt.raw\

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
	emacs $< $(TANGLE_BATCH_SUFFIX) # --batch -f org-babel-tangle --kill

$(SRC_CLASS_DIR)/%.h: $(ORG_CLASS_DIR)/spon-%.org
	emacs $< $(TANGLE_BATCH_SUFFIX) # --batch -f org-babel-tangle --kill

third-party: tiny-xml gtest
clean-third-party:
	rm -rf third-party/include/*
	rm -rf third-party/lib/*

tiny-xml:
	cd third-party/repos/tinyxml2        && \
	$(CC) -c -o tinyxml2.o tinyxml2.cpp  && \
	ar cr libtinyxml2.a tinyxml2.o       && \
	cp libtinyxml2.a ../../lib           && \
	cp tinyxml2.h ../../include

GTEST_CMAKE_FLAGS = -DCMAKE_CXX_COMPILER="$(CC)" -DCMAKE_CXX_FLAGS="-std=c++11" -stdlib=libc++
gtest:
	cd third-party/repos/googletest               && \
	cmake $(GTEST_CMAKE_FLAGS) .                  && \
	make                                          && \
	cp -a googletest/include/gtest ../../include  && \
	cp googlemock/gtest/libgtest.a ../../lib      && \
	cp googlemock/gtest/libgtest_main.a ../../lib

clean:
	rm -f $(EXEC)
