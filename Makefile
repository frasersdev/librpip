DIRS = src/librpip src/pwmclk
EDIRS = src/examples/basic  src/examples/i2c-lcd src/examples/i2c-th02 src/examples/i2c-mpu6500 src/examples/pwm src/examples/spi-mcp3002 src/examples/spi-mcp3202  src/examples/uart

INSTALL	= /usr/local

SO_VERSION = 0
REL_VERSION = 0.4.1

all :
	@set -e; for d in $(DIRS); do $(MAKE) -C $$d -j SO_VERSION=${SO_VERSION} REL_VERSION=${REL_VERSION}; done

examples :
	@set -e; for d in $(EDIRS); do $(MAKE) -C $$d ; done

examplesclean :
	@set -e; for d in $(EDIRS); do $(MAKE) -C $$d clean ; done

install :
	cp include/librpip.h ${INSTALL}/include
	cp lib/librpip.a ${INSTALL}/lib
	cp lib/librpip.so.${REL_VERSION} ${INSTALL}/lib
	rm -f ${INSTALL}/lib/librpip.so.${SO_VERSION}
	ln -s ${INSTALL}/lib/librpip.so.${REL_VERSION} ${INSTALL}/lib/librpip.so.${SO_VERSION}
	mkdir -p ${INSTALL}/bin/librpip-util
	cp bin/* ${INSTALL}/bin/librpip-util

clean :
	rm -f lib/librpip.a lib/librpip.so.* bin/pwmclk config.log
	rm -Rf autom4te.cache
	find . -type f -name '*DS_Store' -exec rm -f '{}' ';'
	find . -type f -name '._*' -exec rm -f '{}' ';'
	@set -e; for d in $(DIRS); do $(MAKE) -C $$d clean ; done
	@set -e; for d in $(EDIRS); do $(MAKE) -C $$d clean ; done
