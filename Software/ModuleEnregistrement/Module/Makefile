CC=gcc
CPP=g++

SRCDIR=src
HEADDIR=header
LIBDIR=obj
BINDIR=bin
 
INCLUDE_DIRS = -I/opt/vc/include -I/opt/vc/include/interface/vcos/ -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vmcs_host -I/opt/vc/include/interface/vchi -I/usr/include/SDL 

# GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)

# Les différentes options de compilations, soyons strictes !
CFLAGS= -O2 -I$(HEADDIR) -Wall $(INCLUDE_DIRS) -D_REENTRANT -DGIT_VERSION=\"$(GIT_VERSION)\"
# Les différents FrameWorks et bibliothèques pour le linkage
GLLIBS = -lm -lpthread -L/opt/vc/lib -lmmal_core -lmmal_util -lmmal_vc_client -lvcos -lbcm_host -lGLESv2 -lEGL

# Où trouver les différents sources *.c qu'il faudra compiler pour créer les objets correspondants
SRC= $(wildcard $(SRCDIR)/*.c)
OBJ= $(SRC:$(SRCDIR)/%.c=$(LIBDIR)/%.o)
 
# Où trouver les différents sources *.cpp qu'il faudra compiler pour créer les objets correspondants
SRCPP= $(wildcard $(SRCDIR)/*.cpp)
OBJCPP= $(SRCPP:$(SRCDIR)/%.cpp=$(LIBDIR)/%.o)
 
all: dir app cpConf
 
#Création de l'exécutable
app: $(OBJ) $(OBJCPP)
	@$(CPP) -o $(BINDIR)/$@ $^ $(CFLAGS) $(GLLIBS)
	
 
# Création des différents *.o à partir des *.c
$(LIBDIR)/%.o: $(SRCDIR)/%.c  
	@$(CC) -o $@ -c $< $(CFLAGS) 
 
# Création des différents *.o à partir des *.cpp
$(LIBDIR)/%.o: $(SRCDIR)/%.cpp 
	$(CPP) -o $@ -c $< $(CFLAGS) 

# Nettoyage des objets => Tout sera recompiler !
clean:
	-@rm -f $(LIBDIR)/*.o
 
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
