del WebDb2ODBC.xpi
rem zip -add -dir=current  iodbc.xpi
pkzipc -add -dir=relative -excl=create.bat -excl=WebDb2ODBC.xpi WebDb2ODBC
ren WebDb2ODBC.zip  WebDb2ODBC.xpi
copy WebDb2ODBC.xpi  ..\install\WebDb2ODBC.xpi
