#CROSS_COMPILE ?= arm-linux-gnueabihf-
CC      = $(CROSS_COMPILE)gcc
AR      = $(CROSS_COMPILE)ar
RM = rm -f
SYS_AP_NAME = am3xtest

CFLAGS = -O2 -Wall -Wno-unused-parameter
LINK_LIB = -lrt -lm -ldl -lpthread

allgen : ${SYS_AP_NAME}

${SYS_AP_NAME} : *.c
	${CC} -c -g ${CFLAGS} *.c
	${RM} ${SYS_AP_NAME}
	${CC} -o ${SYS_AP_NAME} *.o ${LINK_LIB}

clean :
	${RM} *.o ${SYS_AP_NAME}

