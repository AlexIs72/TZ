MOD_NAME=mod_queue

#obj-m      := $(TARGET).o $(TARGET2).o
#	$(TARGET)-objs := $(OBJS)

obj-m := $(MOD_NAME).o
$(MOD_NAME)-objs := mod_main.o queue.o

build: 
	make -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) clean

remove:
	is_module_loaded=`lsmod | grep $(MOD_NAME)`; \
	[ ! -z "$$is_module_loaded" ] && { \
		echo "Remove module"; \
		sudo rmmod $(MOD_NAME); \
	} || true; \
	rm -f /tmp/queue_cache/*

load: remove build
	mkdir -p /tmp/queue_cache	
	sudo insmod $(CURDIR)/$(MOD_NAME).ko