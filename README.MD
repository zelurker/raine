RAINE
=====

OS X requirements
-----------------

In order to run (and compile) Raine on OS X 10.10.x, you need to install mandatory 3rd party libs:

### • Install Homebrew:
Open /Applications/Utilities/Terminal.app and enter the command 
```
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```
See http://brew.sh/ how to keep it up to date
This is a package manager for 3rd party libs

### • Install Libs --universal:
```
brew reinstall sdl --universal
brew reinstall sdl_image --universal
brew reinstall sdl_ttf --universal
brew reinstall xz --universal
brew reinstall sdl_sound --universal
brew reinstall libpng --universal
brew reinstall libtiff --universal
brew reinstall gettext --universal
```
### • Force link to gettext:
```
brew link gettext --force
```

### • Install muparser:
```
brew reinstall muparser --universal
```

### • Install Builders:
If you want to compile, install also gcc and nasm
```
brew install gcc
brew install nasm
```

That's it ! Raine should now run on your MacOS. 
