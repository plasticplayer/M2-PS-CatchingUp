CC=gcc
CPP=g++

SRCDIR=./src
HEADDIR=./header
LIBDIR=obj
BINDIR=bin
DEPDIR=depend
 
INCLUDE_DIRS =  

# GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)

# Les différentes options de compilations, soyons strictes !
#CFLAGS= -I$(HEADDIR) -Wall $(INCLUDE_DIRS) -D_REENTRANT -DGIT_VERSION=\"$(GIT_VERSION)\"
CFLAGS= -I$(HEADDIR) -g -Wall -D_REENTRANT -DGIT_VERSION=\"$(GIT_VERSION)\"  `mysql_config --libs` -std=gnu++11 -lssl -lcrypto 
# Les différents FrameWorks et bibliothèques pour le linkage
GLLIBS = -lm -lpthread

# Où trouver les différents sources *.c qu'il faudra compiler pour créer les objets correspondants
SRC= $(wildcard $(SRCDIR)/*.c)
OBJ= $(SRC:$(SRCDIR)/%.c=$(LIBDIR)/%.o)
 
# Où trouver les différents sources *.cpp qu'il faudra compiler pour créer les objets correspondants
SRCPP= $(wildcard $(SRCDIR)/*.cpp)
OBJCPP= $(SRCPP:$(SRCDIR)/%.cpp=$(LIBDIR)/%.o)
 
DEPLIST= $(wildcard $(DEPDIR)/*.depend)

all: dir app cpConf

# Création de l'exécutable
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


cert:
	@echo "-- Creating Certificates"
	openssl req -new -x509 -out  ./bin/mycert.pem -days 0 -nodes -batch
	@cat privkey.pem >> ./bin/mycert.pem
	@rm -f privkey.pem
	@echo "-- Done Creating Certificates"

dir:
	@mkdir -p $(LIBDIR)
	@mkdir -p $(BINDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(HEADDIR)
	@mkdir -p $(DEPDIR)

-include $(DEPLIST)
