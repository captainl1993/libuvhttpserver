TARGET= ./../build/WXHTTPServer

CFLAGS :=-Wall -Wno-unused -Wno-sign-compare  -ggdb3 -g -Wno-deprecated -std=c++11 \
	 -I ../../Includes/	\
         -I ../../Includes/uv\


cppfiles := $(shell ls *.cpp  )
cfiles := $(shell ls *.c )


COBJS=${patsubst %.c,../obj/%.o,$(cfiles)}
CXXOBJS=${patsubst %.cpp,../obj/%.o,$(cppfiles)}

OBJS := ${COBJS}  ${CXXOBJS}

DEPS=$(patsubst %.o,%.d,$(OBJS))
LIB64 = -L /usr/lib64  -lpthread  -lssl -lcrypto -lz -ldl 
LIB= ../../Lib_3rd/lib/libuv.a\

SO_LIB= 


.PHONY: all clean

all: ${TARGET}
	
${TARGET}: ${OBJS} ${LIB} 
	${CXX} -o $@ $^ ${LDFLAGS} ${SO_LIB} ${LIB} ${LIB64}

${CXXOBJS}:../obj/%.o:../src/%.cpp
	${CXX} -MMD -c -o $@ $< ${CFLAGS} 
	
${COBJS}:../obj/%.o:../src/%.c
	${CC} -MMD -c -o $@ $< ${CFLAGS} 

-include $(DEPS)

clean:
	rm -f ${OBJS} ${TARGET} ${DEPS}

show:
	@echo GPROF=$(GPROF)
	@echo CFLAGS=$(CFLAGS)
	@echo LDFLAGS=$(LDFLAGS)
	@echo objs=$(OBJS)
	@echo cppfiels=$(cppfiles)
	@echo cfiels=$(cfiles)
	@echo DEPS=$(DEPS)
	@echo CXXOBJS=$(CXXOBJS)
	@echo COBJS=$(COBJS)

