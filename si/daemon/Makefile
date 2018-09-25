TARGET = queued

SRC = main.c

$(TARGET):  $(SRC)
	gcc $(SRC) -o $(TARGET)

clean:
	rm -f $(CURDIR)/*.o $(CURDIR)/$(TARGET)

stop:
	$(CURDIR)/queued.sh stop

start: stop
	$(CURDIR)/queued.sh start
