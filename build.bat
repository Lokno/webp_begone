rc.exe /fo app_icon.res app_icon.rc
cl.exe webp_begone.c app_icon.res /Ilibwebp-1.4.0-windows-x64\include /link /LIBPATH:libwebp-1.4.0-windows-x64\lib libwebp.lib