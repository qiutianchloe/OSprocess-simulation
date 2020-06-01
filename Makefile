OBJS = scheduler.o
scheduler: $(OBJS)
	gcc $(OBJS) -o scheduler -lm
scheduler.o: scheduler.c 
	gcc -c scheduler.c -o scheduler.o
clean:
	rm -rf *.o scheduler
