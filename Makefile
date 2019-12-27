EE_BIN = fragmentPatcher.elf
EE_OBJS = fragmentPatcher.o menu.o systemHook.s
EE_LDFLAGS = -L$(PS2SDK)/sbv/lib
EE_LIBS = -lpad -ldebug -lcdvd -lc
all: $(EE_BIN)

clean:
	rm -f *.elf *.o *.a

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal