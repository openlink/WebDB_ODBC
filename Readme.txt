A little about dir structure:

~\Extension  - content extensions sources for Opera, FF, Chrome & Safari
  Opera - build via create.bat_
  FF    - build via create.bat_  
  Chrome - build from Chrome browser only
  Safari - build fro Safari browser only(you must have Safari developer certificate installed before)

~\IE_ActiveX - sources for build IE ActiveX dll and create setup package
   Note you must have installed http://www.firebreath.org and CMake for using this sources.

~\Plugin
~\Plugin\setup  - sources for create plugin setup packages for Linux, MacOSX, Win32(use Inno Setup Compiler Unicode)
~\Plugin\src - plugin source code
    for build Chrome binary(needed for Chrome extension) use:
       - Makefile.Linux_chrome
       - .\ChromeBuildPlugin\NPXMLA_Chrome.xcodeproj
       - npxmla_chrome.sln
    for build plugin for Opera, Safari and another NPAPI browsers
       - Makefile.Linux
       - NPXMLAPlugin.xcodeproj
       - npxmla.sln

