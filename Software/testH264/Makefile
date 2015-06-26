CC=gcc
CPP=g++

SRCDIR=src
HEADDIR=header
LIBDIR=obj
BINDIR=bin
DEPDIR=depend
 
INCLUDE_DIRS =  

# Les différentes options de compilations, soyons strictes !
CFLAGS= -I$(HEADDIR) -O3 -Wall $(INCLUDE_DIRS) -DGIT_VERSION=\"$(GIT_VERSION)\" -pthread -D__STDC_CONSTANT_MACROS
# Les différents FrameWorks et bibliothèques pour le linkage
GLLIBS =  
GLLIBS += -lm 
#GLLIBS += -lssl -lcrypto
GLLIBS += `pkg-config --libs --cflags opencv`
GLLIBS += -lx264 `pkg-config --libs --cflags x264 libswscale libavformat libavutil libavcodec`
# GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)

# Où trouver les différents sources *.c qu'il faudra compiler pour créer les objets correspondants
SRC= $(wildcard $(SRCDIR)/*.c)
OBJ= $(SRC:$(SRCDIR)/%.c=$(LIBDIR)/%.o)
 
# Où trouver les différents sources *.cpp qu'il faudra compiler pour créer les objets correspondants
SRCPP= $(wildcard $(SRCDIR)/*.cpp)
OBJCPP= $(SRCPP:$(SRCDIR)/%.cpp=$(LIBDIR)/%.o)

DEPLIST= $(wildcard $(DEPDIR)/*.depend)

all: dir app  
#Création de l'exécutable
app: $(OBJ) $(OBJCPP)
	@echo "-- Linking App"
	@$(CPP) -o $(BINDIR)/$@ $^ $(CFLAGS) $(GLLIBS)
	@echo "-- End"	
 
# Création des différents *.o à partir des *.c
$(LIBDIR)/%.o: $(SRCDIR)/%.c
	@echo "-- Compiling $<"  
	@$(CC) -o $@ -c $< $(CFLAGS) 
	@echo "-- Generating Dependencies for $<"
	@$(CC) $(CFLAGS) -MT '$@' -MM $< > $(DEPDIR)/$(notdir $<).depend


# Création des différents *.o à partir des *.cpp
$(LIBDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "-- Compiling $<" 
	@$(CPP) -o $@ -c $< $(CFLAGS)
	@echo "-- Generating Dependencies for $<"
	@$(CPP) $(CFLAGS) -MT '$@' -MM $< > $(DEPDIR)/$(notdir $<).depend


# Nettoyage des objets => Tout sera recompiler !
clean:
	-@rm -f $(LIBDIR)/*.o
	-@rm -f $(DEPDIR)/*.depend 
# Nettoyage complet => clean + effacement du l'exécutable
Clean: clean
	-@rm -f $(BINDIR)/*

cpConf:
	@for file in *.ini; do \
		cp "$$file" $(BINDIR)/ -f; \
	done
	
cppCheck:
	cppcheck -j 4 --enable=all --inconclusive --xml --xml-version=2 $(SRCDIR) $(HEADDIR) 2> cppcheck-result.xml

dir:
	@mkdir -p $(LIBDIR)
	@mkdir -p $(BINDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(HEADDIR)
	@mkdir -p $(DEPDIR)


-include $(DEPLIST)
