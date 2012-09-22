@echo off
del .\release /q
del .\debug /q
rd  .\release
rd .\debug

del .\*.ncb /f/q
del .\*.suo /f/q/ah

dir /d /b /ad > subdirs.txt

for /f %%c in (subdirs.txt) do del .\%%c\release /q
for /f %%c in (subdirs.txt) do del .\%%c\debug /q
for /f %%c in (subdirs.txt) do rd .\%%c\release 
for /f %%c in (subdirs.txt) do rd .\%%c\debug
for /f %%c in (subdirs.txt) do attrib +h .\%%c\*.vcproj
for /f %%c in (subdirs.txt) do del .\%%c\*.vcproj.* /q
for /f %%c in (subdirs.txt) do attrib -h .\%%c\*.vcproj

del subdirs.txt