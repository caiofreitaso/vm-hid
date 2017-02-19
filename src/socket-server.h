#include <sys/socket.h>
#include <sys/un.h>
#include <systemd/sd-daemon.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

void
init_socket(struct sockaddr_un* a, const char* path)
{
	memset(a,0,sizeof(struct sockaddr_un));
	a->sun_family = AF_UNIX;
	strncpy(a->sun_path, path, sizeof(a->sun_path));
}

int
get_socket_fd(const char* path)
{
	int fd;
	int n_files = sd_listen_fds(0);

	if (n_files > 1)
	{
		fprintf(stderr, SD_ERR "[VM-HID] Too many file descriptors received.\n");
		return 0;
	}
	else if (n_files == 1)
	{
		fd = SD_LISTEN_FDS_START + 0;
		fprintf(stderr, SD_INFO "[VM-HID] Got file descriptor.\n");
		return fd;
	}
	else
	{
		struct sockaddr addr;
		struct sockaddr_un sock;

		fprintf(stderr, SD_INFO "[VM-HID] Initializing socket...\n");

		if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		{
			fprintf(stderr, SD_ERR "[VM-HID] Failed to initialize socket. Error %d: %s\n", errno, strerror(errno));
			return 0;
		}

		memset(&addr,0,sizeof(struct sockaddr));
		init_socket(&sock, path);
		fprintf(stderr, SD_INFO "[VM-HID] Binding socket...\n");
		if (bind(fd, &addr, sizeof(addr)) < 0)
		{
			fprintf(stderr, SD_ERR "[VM-HID] Failed to bind socket. Error %d: %s\n", errno, strerror(errno));
			return 0;
		}

		if (listen(fd, SOMAXCONN) < 0)
		{
			fprintf(stderr, SD_ERR "[VM-HID] Failed to listen to socket. Error %d: %s\n", errno, strerror(errno));
			return 0;
		}
		return fd;
	}
}

int
get_connection(int fd)
{
	int conn;
	conn = accept(fd, 0, 0);

	if (conn == -1)
		fprintf(stderr, SD_ERR "[VM-HID] Failed to accept connection.\n");

	return conn;
}

int read_connection(int conn, char buffer[128])
{
	int rconn;

	rconn = read(conn, buffer, 128);

	if (rconn == -1)
		fprintf(stderr, SD_ERR "[VM-HID] Failed to read connection.\n");

	return rconn;
}
