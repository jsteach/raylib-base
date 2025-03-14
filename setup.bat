@echo off
IF NOT EXIST "Tools\windows_x64\zig.exe" (
    .\Tools\wget.com https://ziglang.org/download/0.11.0/zig-windows-x86_64-0.11.0.zip
    .\Tools\unzip.com -j zig-windows-x86_64-0.11.0.zip zig-windows-x86_64-0.11.0/zig.exe -d ./Tools/windows_x64
    del .\zig-windows-x86_64-0.11.0.zip
)