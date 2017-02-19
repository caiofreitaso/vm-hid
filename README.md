# VM-HID daemon

This is a server that connects to **libvirt** in order to attach or detach USB devices to/from virtual machines. Got the idea from [this guy](https://rokups.github.io/blog/#!pages/kvm-hid.md) and traslated it to C.

## Dependencies

* *libsystemd-dev*, for the header *sd-daemon.h*
* *libvirt*, to call virsh
* *socat*
* *xset*

## Usage

### Host

In the host you can just call the script:

```
vm-attach machine-name [USB IDs] [v]
```

* **machine-name**: Name for target virtual machine in libvirt
* **USB-IDs**: Optional argument for the list of USB devices to attach to target VM. USB ID are eight hexadecimal digits is in the format: `####:####`
* **v**: Optional argument to turn off video signal at display :0.0

### Guest

In the guest you need a connection to `/run/vm-hid.sock`. In order to do that, you need to add a serial port