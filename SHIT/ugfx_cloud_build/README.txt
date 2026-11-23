UGFX — Windows VST3 for FL Studio

This package includes source code and a GitHub Actions workflow to build the Windows VST3
on a hosted Windows runner. You do NOT need Visual Studio, CMake, or Git installed locally.

See README_RU.txt for step-by-step instructions. The ZIP itself is not a compiled plugin:
you must upload its contents to a GitHub repository, run the workflow under Actions, and
download the UGFX-Windows-VST3 artifact. Copy the resulting UGFX.vst3 folder to
%LOCALAPPDATA%\Programs\Common\VST3\, then scan it in FL Studio.
