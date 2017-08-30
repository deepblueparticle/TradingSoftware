SUBDIRS:=source/CppClient/client

.PHONY: all $(SUBDIRS)
all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ clean all

install:
	@mkdir -p install/include/ib_api/client
	@mkdir -p install/lib
	for dir in $(SUBDIRS); do \
	cp -f $$dir/*.h install/include/ib_api/client 2> /dev/null; \
	cp -f $$dir/*.a install/lib 2> /dev/null; \
	cp -f $$dir/*.so install/lib 2> /dev/null; \
	done

clean:
	rm -rf install
	for dir in $(SUBDIRS); do \
       $(MAKE) -C $$dir clean; \
     done
