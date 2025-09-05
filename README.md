# TYPESUPERFASTJUTSU
<div align="center">
  
  [![C/C++ CI](https://github.com/hexaredecimal/TYPESUPERFASTJUTSU/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/hexaredecimal/TYPESUPERFASTJUTSU/actions/workflows/c-cpp.yml)

</div>

>> A keylogger for x11 that captures the global keypresses on your keyboard and turn them into a cool
>> naruto hand sign animation while leaking them to the screen.

<div style="color:red">
[Note]: This may leak your passwords to your observers. Do not enter sensitive information while this application is being observed by someone.
Stay safe and take care. 
</div>

## Preview

https://github.com/user-attachments/assets/dcf9b757-1317-40f1-88b5-000810d5c392

## Why?
Cool streamers have screen-reader software that allows them to show the keys they are typing on screen. 
I wanted that but with a better visual appeal

## Building
The project uses X11, raylib and nob.h. Make sure X11 and raylib are installed

```bash
$ git clone https://github.com/hexaredecimal/TYPESUPERFASTJUTSU.git
$ cd TYPESUPERFASTJUTSU

# Bootsrap your build system
$ gcc nob.c -o nob

# Build the final executable
$ ./nob
```


## Old Version
This version is a port and improvement of my first [attempt](https://github.com/hexaredecimal/TYPESUPERFASTJUTSU.OLD) at this program. 
Previously I used rust but it was clunky and not fun to develop.


## Rerefences
- [nob.h](https://github.com/tsoding/nob.h)
- [raylib](https://https://raylib.com/)
- [X11](https://www.x.org/releases/X11R7.7/doc/index.html)


