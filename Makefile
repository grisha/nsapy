# Makefile for embedded Python use demo

CC=	gcc
LD=	ld -G

# Target
# for 2.0 servers "_nsapy20.so"
# for 3.0 servers "_nsapy30.so"

TARGET=_nsapy30.so

# Top of the build tree and source tree
blddir=		../adm/Python-1.4/
srcdir=		../adm/Python-1.4/

# Netscape include directory
nsdir=		/usr/ns-home/nsapi/include
#nsdir=		/usr/netscape/suitespot/include

# Compiler flags
OPT=		-g -Wall
INCLUDES=	-I$(srcdir)/Include -I$(blddir) -I$(nsdir)
DEFINES=	-DHAVE_CONFIG_H -DNET_SSL -DSOLARIS -DREENTRANT -DMCC_HTTPD -DXP_UNIX
CFLAGS=		$(OPT) $(DEFINES) $(INCLUDES) 

# Libraries (must be in this order!)
PYLIBS=		$(blddir)/Modules/libModules.a \
		$(blddir)/Python/libPython.a \
		$(blddir)/Objects/libObjects.a \
		$(blddir)/Parser/libParser.a

# XXX edit MODLIBS, LIBS and SYSLIBS to match $(blddir)/Modules/Makefile
MODLIBS=	$(LOCALMODLIBS) $(BASEMODLIBS)
LIBS=		-lsocket -lnsl -ldl
SYSLIBS=	-lm $(LIBM) $(LIBC)
ALLLIBS=	$(PYLIBS) $(MODLIBS) $(LIBS) $(SYSLIBS)

all:		$(TARGET)

$(TARGET):	nsapimod.o config.o getpath.o
		$(LD) nsapimod.o config.o getpath.o $(ALLLIBS) -o $(TARGET)

# Build config.o, suppressing the main() function
config.o:	$(blddir)/Modules/config.c
		$(CC) $(CFLAGS) -DNO_MAIN -c $(blddir)/Modules/config.c

getpath.o:	$(blddir)/Modules/getpath.c
		$(CC) $(CFLAGS) -c $(blddir)/Modules/getpath.c

# Administrative targets


clean:
		-rm -f *.o core

clobber:	clean
		-rm -f *~ @* '#'* _nsapy20.so
