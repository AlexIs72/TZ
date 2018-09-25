TARGETS = client daemon module

build: 
	for i in $(TARGETS); do make -C $$i; done

clean: $(TARGETS)
	for i in $(TARGETS); do make -C $$i clean; done

run: build
	make -C module load
	make -C daemon start
	make -C client run

stop:
	make -C daemon stop
	make -C module remove
