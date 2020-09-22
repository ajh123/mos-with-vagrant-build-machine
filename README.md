## mOS

![license MIT](https://img.shields.io/badge/license-MIT-blue>)
[![By Vietnamese](https://raw.githubusercontent.com/webuild-community/badge/master/svg/by.svg)](https://webuild.community)

mOS is the hobby operating system which is developed from scratch

[![](http://i3.ytimg.com/vi/-I3gCIqPkuU/maxresdefault.jpg)](https://www.youtube.com/watch?v=-I3gCIqPkuU "mOS demo")

### Work-in-process features

- [x] Filesystem
- [x] Program loading
- [x] UI (X11)
- [x] Log
- [x] Networking
- [x] Signal
- [ ] Terminal
- [ ] POSIX compliant
- [ ] Port GCC (the GNU Compiler Collection)
- [ ] Sound
- [ ] Symmetric multiprocessing

🍀 Optional features

- [ ] Setup 2-level paging in boot.asm
- [ ] Dynamic linker

### Get started

```
###. Building
$ brew cask install virtualbox # Only for first time
$ brew cask install vagrant    # /\               /\
$ vagrant up
$ vagrant ssh
$ cd /vagrant/src && ./create_image.sh && ./build.sh qemu iso
```

### If theres a qemu erorr /\
```
On vagrant machine
$ exit
Now on host
$     sudo qemu-system-i386 -s -S -boot c -cdrom mos.iso -hda hdd.img \
      -netdev tap,id=mnet0,ifname=tap0,script=./tapup.sh,downscript=./tapdown.sh -device rtl8139,netdev=mnet0,mac=52:55:00:d1:55:01 \
      -serial stdio -serial file:logs/uart2.log -serial file:logs/uart3.log -serial file:logs/uart4.log \
      -rtc driftfix=slew
```

**Debuging**

in `build.sh`, adding `-s -S` right after `qemu` to switch to debug mode. Currently, I use vscode + [native debuge](https://marketplace.visualstudio.com/items?itemName=webfreak.debug) -> click Run -> choose "Attach to QEMU"

**Monitoring**

```
tail -f serial.log | while read line ; do echo $line ; done
```

✍🏻 Using `tail` in pipe way to color the output (like above) causes a delay -> have to manually saving in ide to get latest changes

### References

#### Tutorials

- http://www.brokenthorn.com/Resources
- http://www.jamesmolloy.co.uk/tutorial_html/
- https://wiki.osdev.org

#### Ebooks

- [Understanding the Linux Kernel, 3rd Edition by Daniel P. Bovet; Marco Cesati](https://learning.oreilly.com/library/view/understanding-the-linux/0596005652/)
