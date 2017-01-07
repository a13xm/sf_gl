SYSCONF_LINK = clang++
CPPFLAGS     = -g -Wall -Wextra -Weffc++ -Wno-gnu-anonymous-struct -Wno-nested-anon-types -pedantic -std=c++11
LDFLAGS      = -g -Wall -O3
LIBS         = -lm

SRCDIR = ./src/
DESTDIR = ./bin/
OBJDIR = ./bin/
OUTDIR = ./out/
TARGET = main

OBJECTS := $(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.o, $(wildcard $(SRCDIR)*.cpp))

all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)
 
$(OBJECTS): $(DESTDIR)%.o: $(SRCDIR)%.cpp
	$(SYSCONF_LINK) $(CPPFLAGS) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(DESTDIR)$(TARGET)
	-rm -f $(OUTDIR)*
