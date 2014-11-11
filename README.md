# Masochist

Masochist is a framework for creating XNU based rootkits. Very useful in OS X and iOS security research. Right now, it isn't much, but I have many plans for this project. These features include:

  - Public symbol resoluton
  - Process manipulation
  - System call hijacking
  - IP filters
  - VFS prodding
  - General kernel hooking
 
Masochist is currently compatible with 64-bit OS X machines (I think). I've only tested this on 10.10. If anyone is brave enough to try it on their system, I would be very grateful. 
 
### Version
0.1


### Usage

Currently, Masochist is a kernel extention itself. Eventually,  it will be a standalone library. However, the current state provides a nice test bed for the early stages of development.

Use Xcode to compile the template. Change target settings if needed.

Once built, run these commands to load the kernel extension

```sh
$ sudo chown -R root:wheel masochist.kext
$ sudo kextload masochist.kext
```

For OS X 10.10, you need to add a boot argument if you don't sign your kext.

```sh
$ sudo nvram boot-args='kext-dev-mode=1'
```


### Disclaimer

Please don't do anything stupid/illegal with this. This stuff can break your Mac. If you're unsure, run OS X in a VM.

