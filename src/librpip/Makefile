# $Id: Makefile,v 1.8 2004/12/04 00:30:34 bsd Exp $

TARGET = librpip

SRCS = board.c device.c sysfs.c dt.c pins.c error.c pwm.c i2c.c spi.c uart.c gpio.c transact.c rpi.c
OBJS = board.o device.o sysfs.o dt.o pins.o error.o pwm.o i2c.o spi.o uart.o gpio.o transact.o rpi.o


OPTIMIZE = -O3
CFLAGS   = -fpic -g -Wall ${OPTIMIZE}

CC     = gcc

all :   depend
	make ${TARGET}

${TARGET} : ${TARGET}.a

.c.s :
	${CC} -c ${CFLAGS} -S -o $@ $<

${TARGET}.a : ${OBJS}
	ar  rcs ${TARGET}.a ${OBJS}
	${CC} -shared -Wl,-soname,${TARGET}.so.${SO_VERSION} -o ${TARGET}.so.${REL_VERSION} ${OBJS} -lc
	mv ${TARGET}.a ../../lib
	mv ${TARGET}.so.${REL_VERSION} ../../lib



clean :
	rm -f *.o *~ .depend Makefile.bak config.h


depend :
	@cp Makefile Makefile.bak
	@awk '/# .Id/,/^# DO NOT DELETE/' Makefile > Makefile.new
	@${CC} ${CFLAGS} -MM ${SRCS} >> Makefile.new
	@if ! diff Makefile Makefile.new > /dev/null 2>&1 ; then \
	  mv Makefile.new Makefile; \
	else \
	  rm Makefile.new; \
	  rm Makefile.bak; \
	fi
	

# DO NOT DELETE (Dependencies follow)
board.o: board.c board.h
device.o: device.c device.h
sysfs.o: sysfs.c sysfs.h
error.o: error.c error.h device.h
pwm.o: pwm.c rpi.h gpio.h error.h sysfs.h board.h pins.h pwm.h
i2c.o: i2c.c rpi.h device.h error.h pins.h i2c.h
spi.o: spi.c rpi.h device.h error.h pins.h transact.h spi.h config.h
uart.o: uart.c rpi.h device.h pins.h error.h transact.h uart.h
gpio.o: gpio.c rpi.h board.h device.h error.h pwm.h gpio.h
transact.o: transact.c rpi.h error.h transact.h i2c.h spi.h uart.h
rpi.o: rpi.c board.h pins.h device.h sysfs.h error.h gpio.h pwm.h \
 transact.h i2c.h spi.h uart.h rpi.h
