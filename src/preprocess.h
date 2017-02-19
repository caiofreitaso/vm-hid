#include <systemd/sd-daemon.h>
#include <string.h>
#include <stdio.h>

struct bf_pos
{
	int vm_start, vm_end;
	int usb_start[12];
	int usb_end[12];
	int num_usb;
	int video;
};

int
is_hex(char a)
{
	return
		((a < 48) || (a > 57)) &&
		((a < 65) || (a > 70)) &&
		((a < 97) || (a > 102));
}

int
is_space(char a)
{
	return a == ' ' || a == '\n';
}

void
init_bfpos(struct bf_pos* a)
{
	memset(a, -1, sizeof(struct bf_pos));
}

struct bf_pos
preprocess(char buffer[128])
{
	struct bf_pos r;

	init_bfpos(&r);


	for(r.vm_start = 1; is_space(buffer[r.vm_start]); r.vm_start++)
		if (buffer[r.vm_start] == 0)
		{
			return r;
		}

	for(r.vm_end = r.vm_start+1; !is_space(buffer[r.vm_end]); r.vm_end++)
		if (buffer[r.vm_end] == 0)
		{
			return r;
		}

	int last = r.vm_end;
	for (; is_space(buffer[last]); last++)
		if (buffer[last] == 0)
		{
			return r;
		}

	for(r.num_usb = 0; r.num_usb < 12; r.num_usb++)
	{
		if (buffer[last] == 0)
		{
			buffer[r.vm_end] = 0;
			r.video = 0;


			return r;
		}

		int i = 0;
		for (; i < 4; i++)
		{
			if (is_hex(buffer[last + i]))
			{
				r.num_usb = -1;
				return r;
			}
		}
		if (buffer[last+4] != ':')
		{
			r.num_usb = -1;
			return r;
		}
		for (i = 5; i < 9; i++)
		{
			if (is_hex(buffer[last + i]))
			{
				r.num_usb = -1;
				return r;
			}
		}

		r.usb_start[r.num_usb] = last;
		r.usb_end[r.num_usb] = last+9;

		last += 9;

		if (buffer[last] != 0)
			for (; is_space(buffer[last]); last++)
				;
	}

	r.video = (buffer[last] == 'v');
	buffer[r.vm_end] = 0;
	return r;
}