# webp_begone
Windows utility to convert all webp files on the Desktop to PNG files

# Usage

Simply double-click the application. It will automatically find your Desktop folder, convert every file with the extension `.webp` to a PNG file of the same name, and delete the original file. 

# Why? 

webp is an excellent image file format that unfortunately still does not have wide-spread support. This utility allows a user to convert files they dragged out of a web browser to PNG files they can immediately use in their non-compliant application.

# Dependencies:

* nothings/stb/stb_image_write.h (https://github.com/nothings/stb)
* libwebp-1.4.0 (https://developers.google.com/speed/webp/download)

# Compilation

```
rc.exe /fo app_icon.res app_icon.rc
cl.exe webp_begone.c app_icon.res /Ilibwebp-1.4.0-windows-x64\include /link /LIBPATH:libwebp-1.4.0-windows-x64\lib libwebp.lib
```
