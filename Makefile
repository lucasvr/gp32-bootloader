# gp32linux boot makefile

all:
	make -C src

clean:
	rm -f *~
	make -C src clean
