# Lo-Ra tools
# Makefile
# Authors:   Marco Boeris Frusca 
#
ARCH ?= x86

ROOTPATH=/

LIB_EXT_PATH ?= /usr/local/lib

INC_EXT_PATH ?= /usr/local/include

ifeq ($(ARCH),x86)

	CPREFIX=

endif

ifeq ($(ARCH),ARM)

	CPREFIX=arm-none-linux-gnueabi-

endif

ifeq ($(ROOTPATH),/)

	LPATH=-L$(LIB_EXT_PATH)
	
else

	LPATH=-L$(ROOTPATH)$(LIB_EXT_PATH)
	
endif

ifeq ($(ROOTPATH),/)

	IPATH=-I$(INC_EXT_PATH)
	
else

	IPATH=-I$(ROOTPATH)$(INC_EXT_PATH)
	
endif

#NAME=lora_config
VERSION=1.0
PREFIX_BIN=/usr/local/bin/
DEBUG=true

CC=$(CPREFIX)g++ -c 
LINK=$(CPREFIX)g++

CFLAGS+=$(IPATH) -Wall -Wno-write-strings -D LORA_NAME=\"$(NAME)\" -D LORA_VERSION=\"$(VERSION)\" 
ifeq ($(NAME),lora_sender)

	CFLAGS+=-D LORA_SENDER=1
endif
ifeq ($(NAME),lora_config)

	CFLAGS+=-D LORA_TEST=1
endif
ifeq ($(NAME),lora_setup)

	CFLAGS+=-D LORA_SETUP=1
endif
 
LFLAGS=$(LPATH) 

IDIRS=-I.	
LDIRS=

LIBS=

SRC=$(wildcard *.cpp) $(wildcard lora/*.cpp)#main.cpp wsn.cpp 

OBJ=$(SRC:.cpp=.o)

all:
	@echo "Compiling variables:"
	@echo "  ARCH         : " $(ARCH)
	@echo "  ROOTPATH     : " $(ROOTPATH)
	@echo "  LIB_EXT_PATH : " $(LIB_EXT_PATH)
	@echo "  INC_EXT_PATH : " $(INC_EXT_PATH)	
	@echo
	@echo "Tempo2: $(NAME) v$(VERSION) - Makefile"
	@echo $(COPY)
	@echo
	@echo "Available targets:"
	@echo "  make debug"
	@echo "  make release"
	@echo "  make build"
	@echo "  make install"
	@echo "  make uninstall"
	@echo "  make clean"
	@echo
	@echo "To cross compile you can use:"
	@echo "  make ARCH=ARM ROOTPATH=<root-fs> LIB_EXT_PATH=<lib-path-of-root-fs> INC_EXT_PATH=<include-path-of-root-fs> <target>"
	@echo

debug: CFLAGS+=-g
debug: LFLAGS+=-g -Wwrite-strings
debug: build

release: CFLAGS+=-Os
release: LFLAGS+=-Os 
release: build

build: $(OBJ)
ifeq ($(ARCH),x86)
	$(LINK) -o $(NAME) $(OBJ) $(LFLAGS) $(LDIRS) $(LIBS)
endif

ifeq ($(ARCH),ARM)
	$(LINK) $(LFLAGS) $(LDIRS) $(LIBS) -o $(NAME) $(OBJ)
endif

%.o: %.cpp
	$(CC) $(CFLAGS) $(IDIRS) $? -o $@
	
install:
#	install -m 755 $(NAME) $(PREFIX_BIN)
#	-mkdir -p $(CFG_DIR)

uninstall:
#	-rm /usr/local/bin/$(NAME)
clean:
	rm -rf $(OBJ) 
