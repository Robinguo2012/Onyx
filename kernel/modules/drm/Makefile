MODULE:= drm
MOD_OBJS:=$(wildcard *.c)
MODDIR:=/usr/lib/modules
CFLAGS+=-Iinclude
all: $(MODULE)
	mkdir -p $(DESTDIR)$(MODDIR)/
	cp $(MODULE).kmod $(DESTDIR)$(MODDIR)/$(MODULE).kmod
$(MODULE): $(MOD_OBJS)
	echo [LD] $@.kmod
	$(CC) -c $(CFLAGS) $(LDFLAGS) $< -o $@.kmod
clean:
	rm $(MODULE).kmod
