WARNINGS :=	-std=c99 -Wpedantic -pedantic-errors \
		-Werror -Wall -Wextra \
		-Wformat=2 \
		-Wcast-qual -Wconversion -Wcast-align \
		-Wswitch-default -Wswitch-enum \
		-Wmissing-prototypes -Wmissing-declarations \
		-Wundef \
		-Wshadow \
		-Wfloat-equal \
		-Waggregate-return \
		-Wunused-parameter -Wunused-but-set-parameter \
		-Wwrite-strings
#		-Wno-format-truncation

FLAGS := -ggdb3 -Og \
	 -DPRIMARY_TEMPLATE_DIR=\"/usr/share/raylib-template\" \
	 -DSECONDARY_TEMPLATE_DIR=\"$(shell realpath ./share)\" \
	 -fsanitize=leak

LINKS :=

.PHONY: all Makefile install uninstall clean

all: main

main: main.c Makefile
	gcc -o $@ $< $(WARNINGS) $(FLAGS) $(LINKS)

clean:
	[[ -e main ]] && rm main

install: main
	cp ./main ./main_stripped
	strip ./main_stripped
	install -m 0755 ./main_stripped /usr/bin/mkhotray
	rm ./main_stripped
	if [[ ! -d /usr/share/mkhotray ]]; then mkdir /usr/share/mkhotray; fi
	install -m 0644 mkhotray.1 /usr/share/man/man1/mkhotray.1
	install -d ./share/src /usr/share/mkhotray/src
	install -d ./share/dist /usr/share/mkhotray/dist
	install -d ./share/src/include /usr/share/mkhotray/src/include
	install -d ./share/src/lib /usr/share/mkhotray/src/lib
	install -m 0644 ./share/Makefile /usr/share/mkhotray/Makefile
	install -m 0644 ./share/src/main.c /usr/share/mkhotray/src/main.c
	install -m 0644 ./share/src/include/header.h \
			/usr/share/mkhotray/src/include/header.h
	install -m 0644 ./share/src/lib/init.c \
			/usr/share/mkhotray/src/lib/init.c
	install -m 0644 ./share/src/lib/close.c \
			/usr/share/mkhotray/src/lib/close.c
	install -m 0644 ./share/src/lib/update.c \
			/usr/share/mkhotray/src/lib/update.c
	install -m 0644 ./share/src/lib/save-restore.c \
			/usr/share/mkhotray/src/lib/save-restore.c

uninstall:
	rm -rf /usr/share/mkhotray
	rm /usr/bin/mkhotray
	rm /usr/share/man/man1/mkhotray.1
