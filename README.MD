RAINE
=====

OS X requirements
-----------------

In order to run (and compile) Raine on OS X 10.10.x, you need to install mandatory 3rd party libs:

### install Homebrew
Open a terminal and enter the command 
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
See http://brew.sh/ how to keep it up to date
This is a package manager for 3rd party libs

### install libs
To install libs, just type "brew install xxxx --universal" in a terminal
where xxx are
* sdl
* sdl_image
* sdl_ttf
* sdl_sound
* xz
* libpng
* libtiff
* gettext
Once done, force link to gettext: type "brew link gettext --force"

* muparser
muparser is also required, but the formulae has no universal option. You need to add it.
type "brew edit muparser", this will open vi text editor
Above the "def install" tag, add "option :universal"
Below the tag, add "ENV.universal_binary if build.universal?"
You should have something like that:
option :universal

  def install
    ENV.universal_binary if build.universal?
    ...

Save the file and install muparser

If you want to compile, install also gcc and nasm

That's it ! Raine should now run on your MacOS. 

### Known bugs
0.64.9
Neogeo games should work, but the 68020 core is still crashing
