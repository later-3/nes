path4o = objs
objs := $(path4o)/main.o $(path4o)/sfc_famicom.o $(path4o)/cpu.o $(path4o)/sfc_mapper.o
program := bin/fc
INCLUDE += include
CFLAGS += -c -Wall -g -I $(INCLUDE)

$(path4o)/%.o:%.c
	$(CC) $(CFLAGS) $^ -o $@

run:$(objs)
	gcc  $(objs) -o $(program)

.PHONY:clean
clean:
	rm -rf $(objs) $(program)