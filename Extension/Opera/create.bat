del Opera_webdb2odbc.oex
rem zip -add -dir=current  iodbc.xpi
pkzipc -add -dir=relative -excl=create.bat -excl=Opera_webdb2odbc.oex Opera_webdb2odbc
ren Opera_webdb2odbc.zip  Opera_webdb2odbc.oex
rem copy iodbc.xpi  ..\install\iodbc.xpi
