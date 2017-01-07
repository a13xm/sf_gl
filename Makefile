SYSCONF_LINK = clang++
CPPFLAGS     = -g -Wall -Wextra -Weffc++ -Wno-gnu-anonymous-struct -Wno-nested-anon-types -pedantic -std=c++11
LDFLAGS      = -g -Wall -O3
LIBS         = -lm

SRCDIR = ./src/
DESTDIR = ./bin/
OBJDIR = ./bin/
OUTDIR = ./out/
TARGET = main
INCDIR = ./src/

OBJECTS := $(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.o, $(wildcard $(SRCDIR)**/*.cpp $wildcard $(SRCDIR)*.cpp))


.PHONY all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)
 
$(OBJECTS): $(DESTDIR)%.o: $(SRCDIR)%.cpp
	@mkdir -p $(@D)
	$(SYSCONF_LINK) $(CPPFLAGS) -I$(INCDIR) -c$(CFLAGS) $< -o $@

.PHONY clean:
	-rm -f $(OBJECTS)
	-rm -f $(DESTDIR)$(TARGET)
	-rm -f $(OUTDIR)*
