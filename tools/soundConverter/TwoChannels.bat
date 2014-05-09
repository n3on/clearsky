@echo off
echo Converting...
ffmpeg2theora.exe -a 10 -A 192 -c 2 --novideo %1
pause