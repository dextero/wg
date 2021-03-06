GAMENAME = WarlocksGauntlet

BINARY = $(GAMENAME).bin32

-include Makefile.sources

CPPFLAGS = -I"build/includes/SFML-1.6/include" -I"build/includes" -DPLATFORM_LINUX
ifdef SCM_REVISION
    CPPFLAGS += -DSCM_REVISION="$(SCM_REVISION)"
endif

LDFLAGS = -L"./libs32"

LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

#default:
CXXFLAGS = -g -Wall

#release:
#release: CXXFLAGS = -O3 -ffast-math
#powyzsze -O3 powodowalo:
#http://img183.imageshack.us/img183/8780/gameplay.png

release: CXXFLAGS = -O2 -static-libgcc
release: OTHER_OBJECTS = /usr/lib/gcc/i686-linux-gnu/4.4/libstdc++.a

#edytor
ifneq ($(findstring editor,$(MAKECMDGOALS)),)
	BINARY_NAME = WGEditor
	CPPFLAGS += -D__EDITOR__ -I"../tools/WGEdytor/wx/include" -I"src" -I "../tools/WGEdytor" -I"../tools/WGEdytor/build/wx-setup"\
		$(shell pkg-config --cflags --libs gtk+-2.0)
	CXXFLAGS += -DUNICODE\
		$(shell wx-config --cppflags)
	LDFLAGS += -L"../tools/WGEdytor/build/lib"
	LIBS += -lwx_baseu-2.8 -lwx_gtk2u_core-2.8 -lwx_gtk2u_stc-2.8 -lwx_gtk2u_html-2.8
endif

CXX = g++

########################

Debug: all

Release: all

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LIBS) $(OTHER_OBJECTS)

%.d: %.cpp
	-$(CXX) -E $(CXXFLAGS)  $(CPPFLAGS) -MM -MP -MF $@ -MQ $(@:.d=.s) -MQ $(@:.d=.o) -MQ $@ $<


clean:
	$(warning Cleaning...)
	@rm -f $(OBJECTS)
	@rm -f $(SOURCES:.cpp=.d)

#czyszczenie plikow edytora
clean-editor: clean

release: all

editor: all

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
-include $(SOURCES:.cpp=.d)
endif

