objs := main.o sfc_famicom.o
program := fc
CFLAGS += -c -Wall -g

%.o:%.c
	$(CC) $(CFLAGS) $^ -o $@

run:$(objs)
	gcc  $(objs) -o $(program)

.PHONY:clean
clean:
	rm -rf *.o fc
