OBJS = schedule.o
schedule: $(OBJS)
	gcc $(OBJS) -o schedule
schedule.o: schedule.c 
	gcc -c schedule.c -o schedule.o
clean:
	rm -rf *.o schedule