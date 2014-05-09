FOR %%i IN (*.dae) DO (
mofConverter.exe %%i %%~ni.mof
)

pause