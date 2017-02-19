all:
	gcc src/main.c -o vm-hid -lsystemd

install: all
	mv vm-hid /usr/local/bin/
	mv systemd/vm-hid.socket /etc/systemd/system/
	mv systemd/vm-hid.service /etc/systemd/system/