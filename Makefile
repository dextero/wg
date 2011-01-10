OLD_BINARY_NAME = warsztat_game
BINARY_NAME = warlocks_gauntlet

-include Makefile.sources

CPPFLAGS = -I"build/includes/SFML-1.4/include" -I"build/includes" -DPLATFORM_LINUX

LDFLAGS = -L"./lib"

LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

#default:
CXXFLAGS = -g -Wall

#release:
#release: CXXFLAGS = -O3 -ffast-math
#powyzsze -O3 powodowalo:
#http://img183.imageshack.us/img183/8780/gameplay.png

release: CXXFLAGS = -O2

iamtoxic: CXXFLAGS += -D__I_AM_TOXIC__

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





#cross kompilacja
ifneq ($(findstring cross-compile,$(MAKECMDGOALS)),)
  BINARY_SUFFIX = .exe
  CPPFLAGS = -I"../SFML-1.4-cross/include" 
  LDFLAGS = -L"../SFML-1.4-cross/lib/mingw"
  CXX = i686-pc-mingw32-g++
endif

OLD_BINARY = $(OLD_BINARY_NAME)$(BINARY_SUFFIX)
BINARY = $(BINARY_NAME)$(BINARY_SUFFIX)

########################

Debug: all

Release: all

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS)

%.d: %.cpp
	-$(CXX) -E $(CXXFLAGS)  $(CPPFLAGS) -MM -MP -MF $@ -MQ $(@:.d=.s) -MQ $(@:.d=.o) -MQ $@ $<


clean:
	$(warning Cleaning...)
	@$(RM) $(BINARY)
	@$(RM) $(BINARY).exe
	@$(RM) $(OLD_BINARY)
	@$(RM) $(OLD_BINARY).exe
	@rm -f $(OBJECTS)
	@rm -f $(SOURCES:.cpp=.d)

#czyszczenie plikow edytora
clean-editor: clean

deploy:
	rm -rf deploy
	rm -rf wg-r*
	mkdir deploy
	svn export data deploy/data
	svn export user deploy/user
	cp *.dll $(BINARY).exe deploy
	mv deploy wg-r`svn info | grep Revision | cut -d" " -f2`-`date +%F_%H-%M-%S`
	zip -9 -r `ls | grep wg-r`.zip wg-r*
	scp wg-*.zip zodiac:public_html/wg
	rm -rf wg-*

test:	$(BINARY)
ifneq ($(findstring cross-compile,$(MAKECMDGOALS)),)
	wine $(BINARY)
else
	./$(BINARY)
endif

cross-compile: all

release: all

iamtoxic: all

editor: all

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
-include $(SOURCES:.cpp=.d)
endif

