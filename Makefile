it:	all

libs  :=	lies.so

all:	$(libs)
clean:
	rm -f $(libs)

.SUFFIXES:	.so
.c.so:
	cc -pipe -Wall -pedantic -Os -s -shared -fPIC -o $@ $<
