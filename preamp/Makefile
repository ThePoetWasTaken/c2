.PHONY: run debug release clean


mips-release:
	make -f Makefile.mips release

mips-debug:
	make -f Makefile.mips debug


x64:
	make -f Makefile.x64 release

release: x64

run: 
	make -f Makefile.x64 run

debug: 
	make -f Makefile.x64 debug

clean: 
	make -f Makefile.x64 clean