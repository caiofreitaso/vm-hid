#include <systemd/sd-daemon.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int create_xml(const char* device, const char* filename);
int attach_device(const char* device, const char* vm);
int detach_device(const char* device, const char* vm);

//create XML for "virsh attach-device"
//device must be in this format "0000:0000"
int
create_xml(const char* device, const char* filename)
{
	char xml[108] = "<hostdev mode='subsystem' type='usb'><source><vendor id='0x0000'/><product id='0x0000'/></source></hostdev>";

	//put device into XML
	{
		int pos1 = 59, pos2 = 81;
		strncpy(&xml[pos1], device, 4); //vendor
		strncpy(&xml[pos2], &device[5], 4); //product
	}

	//save XML
	{
		FILE* output;
		output = fopen(filename, "w+");
		if (output)
		{
			fprintf(output, "%s",xml);
			fclose(output);
		}
		else
		{
			fprintf(stderr, SD_ERR "[VM-HID] Failed to open %s. ERROR %d: %s.\n", filename, errno, strerror(errno));
			return 0;
		}
	}
	return 1;
}

//check if file exists and can be read
int
get_xml(const char* device, const char* filename)
{
	if (access(filename, R_OK) != -1)
		return 1;
	else
		return create_xml(device, filename);
}

int
call_virsh(const char* action, const char* vm, const char* device)
{
	char filename[20] = "/tmp/d0000:0000.xml";

	//put device into filename
	strncpy(&filename[6], device, 9);

	if (get_xml(device,filename))
	{
		char call[256] = "virsh ";

		strcat(call, action);
		strcat(call, " ");
		strcat(call, vm);
		strcat(call, " ");
		strcat(call, filename);

		return (system(call)+1);
	}
	else
	{
		fprintf(stderr, SD_ERR "[VM-HID] Failed to %s %s in %s.\n", action, device, vm);
		return 0;
	}
}

int
turn_off_video()
{
	return system("xset -display :0.0 dpms force off");
}