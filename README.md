# Masochist

Masochist is a framework for creating XNU based rootkits. Very useful in OS X and iOS security research. It can do cool things like:

  - Public symbol resoluton
  - Process manipulation
  - System call hijacking

Masochist is currently compatible with 64-bit OS X machines (I think). I've only tested this on 10.10. If anyone is brave enough to try it on their system, I would be very grateful.

### Version
0.1


### Usage

To use this code, please import these files into your kernel extension project in Xcode. Then, import the headers that you need to use.

I recommend [this repo](https://github.com/squiffy/Masochist-Template) as a starting point for your project. It has been configured and is ready to be built inside Xcode.

The API docs can be found on the Wiki page.


### Disclaimer

Please don't do anything stupid/illegal with this. This stuff can break your Mac. If you're unsure, run OS X in a VM.

