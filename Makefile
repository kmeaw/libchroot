CFLAGS=-fPIC -fpic -W -Wall
LDFLAGS=-shared -ldl

all: libchroot.so

libchroot.so: libchroot.o
	$(CC) $(LDFLAGS) -o $@ $<

clean:
	rm -f libchroot.o libchroot.so

test: libchroot.so
	sudo env LD_PRELOAD=$(PWD)/libchroot.so CHROOT=/var bash -c 'echo /*; echo $${UID}'

.PHONY: clean all test
