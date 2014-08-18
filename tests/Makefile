FIRMWARE_DIR = ../Project/src
FIRMWARE_SRC = command.c move.c extruder.c heatbed.c pid.c
FIRMWARE_OBJS=$(patsubst %.c,firmware_%.o,$(FIRMWARE_SRC))

EMU_SRC = systick.c common.c gfiles.c limitSwitch.c motor.c machinery.c
EMU_OBJS=$(patsubst %.c,emu_%.o,$(EMU_SRC))

CFLAGS += -std=gnu99 -Wno-format-extra-args -pthread
EXTRA_INC = -I.

CC = gcc-4.9

test_move: test_move.o $(FIRMWARE_OBJS) $(EMU_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS) 
firmware_%.o: $(FIRMWARE_DIR)/%.c
	$(CC) -c $^ -o $@ $(CFLAGS) $(EXTRA_INC)
emu_%.o: emu_%.c
	$(CC) -c $^ -o $@ $(CFLAGS) $(EXTRA_INC)
test_%.o: test_%.c
	$(CC) -c $^ -o $@ $(CFLAGS) $(EXTRA_INC)
clean:
	rm *.o