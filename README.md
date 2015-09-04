Run executables with dynamic library dependencies without copying them into chroot

= How to use?

Compile, set `LD_PRELOAD` environment variable to the path of `libchroot.so` and
specify the chroot target directory in `CHROOT`. It will detect sudo environment
and drop privileges to values, specified in `SUDO_UID` and `SUDO_GID`.

`sudo env LD_PRELOAD=$(PWD)/libchroot.so CHROOT=/var ls /`

