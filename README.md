# tihsOS-64

64 bit OS.

Install the following dependencies using your package manager (or aur helper if you use arch btw)
```
mtools
xorriso
nasm
grub-pc-bin
grub-common

ld (should be installed by default)
gcc (should be installed by default)
```

**If you want to build this using Windows, I suggest using WSL2. \
It's what I use when im on Windows. Ubuntu WSL2.**

Build the ISO
```sh
$ make build-x86_64
```

Run the ISO
```sh
$ ./run-os
```

