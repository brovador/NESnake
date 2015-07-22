.PHONY: clean all

.PRECIOUS: *.o

all: ../build/game.nes

run: ../build/game.nes
	../emulator/Macifom.app/Contents/MacOS/Macifom ../build/game.nes

clean:
	@rm -fv game.s
	@rm -fv game.o
	@rm -fv ../build/game.nes
	@rm -fv crt0.o
	@rm -fv level.h

crt0.o: crt0.s
	ca65 crt0.s

%.o: %.c
	python ../tools/nam-to-c.py level.nam level.h
	cc65 -Oi $< --add-source 
	ca65 $*.s
	rm $*.s

../build/%.nes: %.o crt0.o
	ld65 -C nes.cfg -o $@ crt0.o $< runtime.lib c64.lib