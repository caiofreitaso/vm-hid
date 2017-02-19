#include "socket-server.h"
#include "preprocess.h"
#include "calls.h"

void do_action(const char* action, char buffer[128])
{
	struct bf_pos pos = preprocess(buffer);

	int i;
	for (i = 0; i < pos.num_usb; i++)
	{
		fprintf(stderr, SD_INFO "[VM-HID] Calling virsh...\n");
		int ret = call_virsh(action, &buffer[pos.vm_start], &buffer[pos.usb_start[i]]);
		if (ret < 0)
			fprintf(stderr, SD_ERR "[VM-HID] Failed to call virsh. Error %d: %s\n", errno, strerror(errno));
	}

	if (pos.video)
	{
		fprintf(stderr, SD_INFO "[VM-HID] Turning video off...\n");
		turn_off_video();
	}
}

int main(int argc, char const *argv[])
{
	int fd;

	if (fd = get_socket_fd("/run/vm-hid.sock"))
		sd_notify(0, "READY=1\nSTATUS=Initialized socket.");
	else
	{
		fprintf(stderr, SD_ERR "[VM-HID] Failed to initialize socket.\n");
		sd_notifyf(0, "STOPPING=1\nERRNO=%d\nSTATUS=Failed to initialize socket.", errno);
		return 1;
	}

	while(1)
	{
		int conn = get_connection(fd);

		if (conn != -1)
		{
			char buffer[128] = {0};
			int rconn = read_connection(conn, buffer);

			if (rconn != -1)
			{
				if (buffer[0] == 'd')
					do_action("detach-device", buffer);
				else if (buffer[0] == 'a')
					do_action("attach-device", buffer);
				else
				{
					fprintf(stderr, SD_ERR "[VM-HID] Invalid signal. Aborting.\n");
					close(fd);
					sd_notify(0, "STOPPING=1");
					return 1;
				}
			}
			else
			{
				close(fd);
				sd_notify(0, "STOPPING=1");
				return 1;
			}
		}
		else
		{
			close(fd);
			sd_notify(0, "STOPPING=1");
			return 1;
		}
	}

	close(fd);
	sd_notify(0, "STOPPING=1");
	return 0;
}