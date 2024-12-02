# Compiler options
WARNINGS := -std=c99 -Wall
DEV_WARNINGS := -std=c99 -Wpedantic -pedantic-errors \
		-Werror -Wall -Wextra \
		-Wformat=2 \
		-Wcast-qual -Wconversion -Wcast-align \
		-Wswitch-default -Wswitch-enum \
		-Wundef \
		-Wshadow \
		-Wfloat-equal \
		-Waggregate-return \
		-Wunused-parameter -Wunused-but-set-parameter \
		-Wwrite-strings
MACROS := -D::IDENT_UPPER::
DEV_MACROS := -D::IDENT_UPPER::_DEBUG -D::IDENT_UPPER::_HOT_RELOAD -D::LIB_UPPER::_PATH=\"$(shell realpath ./dist/::LIB::.so)\"
SHARED_FLAGS := -shared -fPIC
DEBUG_FLAGS := -ggdb3 -Og -fPIC
LIBRARIES := raylib
FLAGS := $(shell pkg-config --cflags $(LIBRARIES) | sed "s/-I/-isystem /g")
LINKS := $(shell pkg-config --libs $(LIBRARIES)) -lm

# Directories, Files
DIST := ./dist
$(DIST):
	mkdir $(DIST)
SRC := ./src
::LIB_UPPER::_SRC := $(SRC)/lib
INLCUDE := ./src/include
HEADERS := $(wildcard $(INCLUDE)/*.h)
SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(DIST)/%.o, $(SOURCES))
::LIB_UPPER::_SOURCES := $(wildcard $(::LIB_UPPER::_SRC)/*.c)
::LIB_UPPER::_OBJECTS := $(patsubst $(::LIB_UPPER::_SRC)/%.c, $(DIST)/%.o, $(::LIB_UPPER::_SOURCES))
::LIB_UPPER:: := $(DIST)/::LIB::.so

# Change variables depeing on if this a development build or
# production build
ifeq ($(filter dev, $(MAKECMDGOALS)), dev)
	WARNINGS += $(DEV_WARNINGS)
	MACROS += $(DEV_MACROS)
	FLAGS += $(DEBUG_FLAGS)
else ifeq ($(filter reload, $(MAKECMDGOALS)), reload)
	WARNINGS += $(DEV_WARNINGS)
	MACROS += $(DEV_MACROS)
	FLAGS += $(DEBUG_FLAGS)
else # all, prod
# Essentially treats libryz objects like regular objects
# so that they can be linked normally
	OBJECTS += $(::LIB_UPPER::_OBJECTS)
	SHARED_FLAGS :=
endif

# Targets
.PHONY: all clean dev prod reload Makefile

prod: all

dev: $(::LIB_UPPER::) all

all: $(DIST)/::IDENT:: $(HEADERS) Makefile | $(DIST)

reload: $(::LIB_UPPER::)

$(DIST)/::IDENT::: $(OBJECTS) $(HEADERS)
	gcc -o $@ $(OBJECTS) $(WARNINGS) $(MACROS) $(LINKS) $(FLAGS)

$(::LIB_UPPER::): $(::LIB_UPPER::_OBJECTS) $(HEADERS)
	gcc -o $@ $(::LIB_UPPER::_OBJECTS) $(WARNINGS) $(MACROS) $(SHARED_FLAGS) $(LINKS) $(FLAGS)
	chmod -x $(::LIB_UPPER::)	# Not sure why it produces an executable (all it does is segfault)

$(DIST)/%.o: $(::LIB_UPPER::_SRC)/%.c
	gcc -o $@ -c $< $(WARNINGS) $(MACROS) $(LINKS) $(FLAGS)

$(DIST)/%.o: $(SRC)/%.c
	gcc -o $@ -c $< $(WARNINGS) $(MACROS) $(LINKS) $(FLAGS)

clean:
	rm $(DIST)/*
