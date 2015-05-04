CC=gcc
CPP=g++

SRCDIR=src
HEADDIR=header
LIBDIR=obj
BINDIR=bin
DEPDIR=depend
 
INCLUDE_DIRS = -I/opt/vc/src/hello_pi/libs/ilclient -I/opt/vc/include -I/opt/vc/include/interface/vcos/ -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vmcs_host -I/opt/vc/include/interface/vchi -I/usr/include/SDL 


# Les différentes options de compilations, soyons strictes !
CFLAGS= -I$(HEADDIR) -O3 -Wall $(INCLUDE_DIRS) -DGIT_VERSION=\"$(GIT_VERSION)\" -march=armv7-a -pthread
# Les différents FrameWorks et bibliothèques pour le linkage
GLLIBS = -L/opt/vc/lib -L/opt/vc/src/hello_pi/libs/ilclient 
GLLIBS += -lopenmaxil -lbcm_host -lvchiq_arm -lilclient -lrt -lm -lmmal_core -lmmal_util -lmmal_vc_client -lvcos -lGLESv2 -lEGL
# GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)

# Où trouver les différents sources *.c qu'il faudra compiler pour créer les objets correspondants
SRC= $(wildcard $(SRCDIR)/*.c)
OBJ= $(SRC:$(SRCDIR)/%.c=$(LIBDIR)/%.o)
 
# Où trouver les différents sources *.cpp qu'il faudra compiler pour créer les objets correspondants
SRCPP= $(wildcard $(SRCDIR)/*.cpp)
OBJCPP= $(SRCPP:$(SRCDIR)/%.cpp=$(LIBDIR)/%.o)

DEPLIST= $(wildcard $(DEPDIR)/*.depend)

all: dir app cpConf
 
#Création de l'exécutable
app: $(OBJ) $(OBJCPP)
	@echo "-- Linking App"
	@$(CPP) -o $(BINDIR)/$@ $^ $(CFLAGS) $(GLLIBS)
	@echo "-- End"	
 
# Création des différents *.o à partir des *.c
$(LIBDIR)/%.o: $(SRCDIR)/%.c
	@echo "-- Compiling $<"  
	@$(CC) -o $@ -c $< $(CFLAGS) 
	@echo "-- Generating Dependecies for $<"
	@$(CC) $(CFLAGS) -MT '$@' -MM $< > $(DEPDIR)/$(notdir $<).depend


# Création des différents *.o à partir des *.cpp
$(LIBDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "-- Compiling $<" 
	@$(CPP) -o $@ -c $< $(CFLAGS)
	@echo "-- Generating Dependecies for $<"
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
