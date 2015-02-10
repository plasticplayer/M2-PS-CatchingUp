CC=gcc
CPP=g++

SRCDIR=./
HEADDIR=./
LIBDIR=obj
BINDIR=bin
 
 
# Les différentes options de compilations, soyons strictes !
CFLAGS= -I$(HEADDIR) -Wall
# Les différents FrameWorks et bibliothèques pour le linkage
GLLIBS = -lm
 
# Où trouver les différents sources *.c qu'il faudra compiler pour créer les objets correspondants
SRC= $(wildcard $(SRCDIR)/*.c)
OBJ= $(SRC:$(SRCDIR)/%.c=$(LIBDIR)/%.o)
 
# Où trouver les différents sources *.cpp qu'il faudra compiler pour créer les objets correspondants
SRCPP= $(wildcard $(SRCDIR)/*.cpp)
OBJCPP= $(SRCPP:$(SRCDIR)/%.cpp=$(LIBDIR)/%.o)
 
all: dir Sonde Router Coordinateur cpConf cppCheck
 
#Création de l'exécutable
Sonde: $(OBJCPP) $(LIBDIR)/main_sonde.o $(LIBDIR)/parserConfig.o $(LIBDIR)/simSonde.o $(LIBDIR)/GestionReseau.o $(LIBDIR)/ListeChainee.o $(LIBDIR)/utils.o 
	@$(CCP) -o $(BINDIR)/$@ $^ $(CFLAGS) $(GLLIBS)
	
 
# Création des différents *.o à partir des *.c
$(LIBDIR)/%.o: $(SRCDIR)/%.c  
	$(CC) -o $@ -c $< $(CFLAGS)
 
# Création des différents *.o à partir des *.cpp
$(LIBDIR)/%.o: $(SRCDIR)/%.cpp 
	$(CPP) -o $@ -c $< $(CFLAGS)
 
# Nettoyage des objets => Tout sera recompiler !
clean:
	rm $(LIBDIR)/*.o
 
# Nettoyage complet => clean + effacement du l'exécutable
Clean: clean
	rm $(BINDIR)/*

cpConf:
	@for file in config_*; do \
		cp "$$file" $(BINDIR)/ -f; \
	done
	
cppCheck:
	cppcheck -j 4 --enable=all --inconclusive --xml --xml-version=2 $(SRCDIR) $(HEADDIR) 2> cppcheck-result.xml

dir:
	mkdir -p $(LIBDIR)
	mkdir -p $(BINDIR)