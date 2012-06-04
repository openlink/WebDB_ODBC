var $nsODBC;
(function (){

var $ODBCUtils = null;

$nsODBC = function () {
};

$nsODBC.prototype = {
  openDatabaseSync : function (name, version, displayName, estimateSize, creationCallback) {
    var conn = null;
    try {
      if (typeof($ODBCUtils)=="undefined" || $ODBCUtils==null)
        $ODBCUtils = new ActiveXObject("openlinksw.WebDB_to_ODBC.1");
      conn = $ODBCUtils.CreateConnection();
      conn.Open(name);
    } catch (ex) {
      if (conn.errorCode != 0)
        throw new $nsSQLException(conn);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
          throw new $nsSQLException(null, ex.message);
      else
        throw ex;
    }

    var conn_ver = conn.schemaVersion;
    if (typeof(version) == "string" && version.length > 0) {
      var dbver = parseInt(version);
      if (dbver != conn_ver)
        throw "Wrong version database:"+conn_ver;
    }
    var db = new $nsDatabaseSync(conn, conn_ver);
    if (creationCallback) {
      creationCallback.handleEvent(db);
    }
    return db;
  },

  openDatabase : function (name, version, displayName, estimateSize, creationCallback) {
    var conn = null;
    try {
      if (typeof($ODBCUtils)=="undefined" || $ODBCUtils==null)
        $ODBCUtils = new ActiveXObject("openlinksw.WebDB_to_ODBC.1");
      conn = $ODBCUtils.CreateConnection();
      conn.Open(name);
    } catch (ex) {
      if (conn.errorCode != 0)
        throw new $nsSQLException(conn);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        throw new $nsSQLException(null, ex.message);
      else
        throw ex;
    }

    var conn_ver = conn.schemaVersion;
    if (typeof(version) == "string" && version.length > 0) {
      var dbver = parseInt(version);
      if (dbver != conn_ver)
        throw "Wrong version database:"+conn_ver;
    }
    var db = new $nsDatabase(conn, conn_ver);
    if (creationCallback) {
      creationCallback.handleEvent(db);
    }
    return db;
  },

  getDataSources : function() {
    var val;
    var ret = [];
    var i = 0;

    if (typeof($ODBCUtils)=="undefined" || $ODBCUtils==null)
      $ODBCUtils = new ActiveXObject("openlinksw.WebDB_to_ODBC.1");

    val = $ODBCUtils.getDataSource(true);
    if (val != null) {
      ret[i++] = val;
      while(val != null) {
        val = $ODBCUtils.getDataSource(false);
        if (val != null)
          ret[i++] = val;
      }
    }
    return ret;
  }

};



function $nsDatabaseSync(conn, conn_ver) {
  this._conn = conn;
  this.version = ""+conn_ver;
}

$nsDatabaseSync.prototype = {
  _conn: null,
  version: "0",

  transaction : function (callback) {
    if (!this._conn)
        throw "Connection isn't opened";

    if (!callback)
      throw "Transaction callback function is null";

    var msc = new $nsSQLTransactionSync(this, this._conn);
    if (!msc)
       throw "Error out of Memory";

    try {
       this._conn.BeginTransaction();
       this._conn.readOnlyMode = false;

       callback.handleEvent(msc);

       this._conn.CommitTransaction();
    } catch (ex) {
       if (!(ex instanceof $nsSQLException) && this._conn.errorCode != 0)
          ex = new $nsSQLException(this._conn);
       else if (ex instanceof Error && ex.message!="$DB$") 
          ex = new $nsSQLException(null, ex.message);

       try {
         this._conn.RollbackTransaction();
       } catch (e) {}
       throw ex;
    } finally {
       delete msc;
    }
  },


  readTransaction : function(callback) {
    if (!this._conn)
        throw "Connection isn't opened";

    if (!callback)
      throw "Transaction callback function is null";

    var msc = new $nsSQLTransactionSync(this, this._conn);
    if (!msc)
       throw "Error out of Memory";

    try {
       this._conn.BeginTransaction();
       this._conn.readOnlyMode = true;

       callback.handleEvent(msc);

       this._conn.CommitTransaction();
    } catch (ex) {
       if (!(ex instanceof $nsSQLException) && this._conn.errorCode != 0)
          ex = new $nsSQLException(this._conn);
       else if (ex instanceof Error && ex.message!="$DB$") 
          ex = new $nsSQLException(null, ex.message);

       try {
         this._conn.RollbackTransaction();
       } catch (e) {}
       throw ex;
    } finally {
       delete msc;
       this._conn.readOnlyMode = false;
    }
  },


  changeVersion : function (oldVersion, newVersion, callback) {
   var oldVer = parseInt(oldVersion);
   var newVer = parseInt(newVersion);

   if (oldVer != this.version)
     throw "Database version isn't equal connection version";

   var msc = new $nsSQLTransactionSync(this, this._conn);
   if (!msc)
     throw "Error out of Memory";

   try {
      this._conn.BeginTransaction();

      if (callback)
        callback.handleEvent(msc);

      this._conn.schemaVersion = newVer;
      this.version = newVer;

      this._conn.CommitTransaction();
   } catch (ex) {
      if (!(ex instanceof $nsSQLException) && this._conn.errorCode != 0)
         ex = new $nsSQLException(this._conn);
      else if (ex instanceof Error && ex.message!="$DB$") 
         ex = new $nsSQLException(null, ex.message);

      try {
        this._conn.RollbackTransaction();
      } catch (e) {}
      throw ex;
   } finally {
      delete msc;
   }

  }

};


function $nsDatabase(conn, conn_ver) {
  this._conn = conn;
  this.version = ""+conn_ver;
}


$nsDatabase.prototype = {

  //val: "<default value>"
  _conn: null,
  version: "0",


  transaction : function (callback, errorCallback, successCallback) {
    if (!this._conn)
        throw "Connection isn't opened";

    if (!callback)
      throw "Transaction callback function is null";

    this._conn.ExecAsync({
      _self: this,
      run : function() {

        var msc = new $nsSQLTransaction(this._self._conn);

        try {
          this._self._conn.BeginTransaction();
          this._self._conn.readOnlyMode = false;

          callback.handleEvent(msc);
          if (msc._lastError)
            throw msc._lastError;

          this._self._conn.CommitTransaction();

          if (successCallback)
            successCallback.handleEvent();

        } catch (ex) {
          var err = null;
          if (this._self._conn.errorCode != 0)
            err = new $nsSQLError(handle);
          else
          if (ex instanceof Error && ex.message!="$DB$") 
            err = new $nsSQLError(null, ex.message);
          else
            err = new $nsSQLError(null, ex.toString());

          try {
            this._self._conn.RollbackTransaction();
          } catch (e) {}

          if (errorCallback)
            errorCallback.handleEvent(err);
        } finally {
          delete msc;
        }
      }
    });

  },


  readTransaction : function(callback, errorCallback, successCallback) {
    if (!this._conn)
        throw "Connection isn't opened";

    if (!callback)
      throw "Transaction callback function is null";

    this._conn.ExecAsync({
      _self: this,
      run : function() {

        var msc = new $nsSQLTransaction(this._self._conn);

        try {
          this._self._conn.BeginTransaction();
          this._self._conn.readOnlyMode = true;

          callback.handleEvent(msc);
          if (msc._lastError)
            throw msc._lastError;

          this._self._conn.CommitTransaction();

          if (successCallback) 
            successCallback.handleEvent();

        } catch (ex) {
          var err = null;
          if (this._self._conn.errorCode != 0)
            err = new $nsSQLError(handle);
          else
          if (ex instanceof Error && ex.message!="$DB$") 
            err = new $nsSQLError(null, ex.message);
          else
            err = new $nsSQLError(null, ex.toString());

          try {
            this._self._conn.RollbackTransaction();
          } catch (e) {}

          if (errorCallback)
            errorCallback.handleEvent(err);
        } finally {
          delete msc;
          this._self._conn.readOnlyMode = false;
        }
      }
    });
  },


  changeVersion : function (oldVersion, newVersion, callback, errorCallback, successCallback) {
   var oldVer = parseInt(oldVersion);
   var newVer = parseInt(newVersion);

   if (oldVer != this.version)
     throw "Database version isn't equal connection version";

   this._conn.ExecAsync({
       _self: this,
       run : function() {
          var msc = new $nsSQLTransaction(this._self._conn);

          try {
            this._self._conn.BeginTransaction();
            if (callback)
              callback.handleEvent(msc);
            if (msc._lastError)
              throw msc._lastError;

            this._self._conn.schemaVersion = newVer;
            this._self.version = newVer;

            this._self._conn.CommitTransaction();

            if (successCallback)
              successCallback.handleEvent();
          
          } catch (ex) {
            var err = null;
            if (ex instanceof $nsSQLError)
              err = ex;
            else
            if (ex instanceof Error && ex.message!="$DB$") 
              err = new $nsSQLError(null, ex.message);
            else
              err = new $nsSQLError(this._self._conn);

            try {
              this._self._conn.RollbackTransaction();
            } catch (e) {}

            if (errorCallback)
              errorCallback.handleEvent(err);
          } finally {
            delete msc;
          }
       }
   });
  }

};



function $nsSQLException(handle,message) {
  if (handle != null) {
    this.code = handle.errorCode;
    this.message = handle.errorMessage;
    this.state = handle.errorState;
  } else {
    this.code = -1;
    this.message = message;
    this.state = "HY000";
  }
}

$nsSQLException.prototype = {
  code: 0,
  message: null,
  state: null,

  toString : function() {
    return this.message;
  }
};


function $nsSQLError(handle, message) {
  if (handle != null) {
    this.code = handle.errorCode;
    this.message = handle.errorMessage;
    this.state = handle.errorState;
  } else {
    this.code = -1;
    this.message = message;
    this.state = "HY000";
  }
}

$nsSQLError.prototype = {

  code: 0,
  message: null,
  state: null,

  toString : function() {
    return this.message;
  }
};



function $nsSQLTransactionSync(tran, conn) {
  this._tran = tran;
  this._conn = conn;
}

$nsSQLTransactionSync.prototype = {

  _conn: null,
  _tran: null,

  executeSql : function (sqlStatement, arguments) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();

      var arg_len  = (arguments != null && typeof(arguments)!="undefined" ? arguments.length: 0);
      if (arg_len > 0) {
        for (i=0; i < arg_len; i++) {
          stmt.AddParameter(arguments[i]);
        }
      }
      stmt.Execute(sqlStatement);

      return new $nsSQLResultSet(this._conn, stmt);

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      if (handle.errorCode != 0)
        throw new $nsSQLException(stmt?stmt:this._conn);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        throw new $nsSQLException(null, ex.message);
      else
        throw ex;
    }
  },

  getCatalogs : function () {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getTables('%',"","","");
      return new $nsSQLResultSet(this._conn, stmt);

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      if (handle.errorCode != 0)
        throw new $nsSQLException(stmt?stmt:this._conn);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        throw new $nsSQLException(null, ex.message);
      else
        throw ex;
    }
  },

  getTables : function (catalog, schema, table, tableType) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getTables(catalog, schema, table, tableType);
      return new $nsSQLResultSet(this._conn, stmt);

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      if (handle.errorCode != 0)
        throw new $nsSQLException(stmt?stmt:this._conn);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        throw new $nsSQLException(null, ex.message);
      else
        throw ex;
    }
  },

  getColumns : function (catalog, schema, table, column) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getColumns(catalog, schema, table, column);
      return new $nsSQLResultSet(this._conn, stmt);

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      if (handle.errorCode != 0)
        throw new $nsSQLException(stmt?stmt:this._conn);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        throw new $nsSQLException(null, ex.message);
      else
        throw ex;
    }
  },
  
  getPrimaryKeys : function (catalog, schema, table) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getPrimaryKeys(catalog, schema, table);
      return new $nsSQLResultSet(this._conn, stmt);

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      if (handle.errorCode != 0)
        throw new $nsSQLException(stmt?stmt:this._conn);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        throw new $nsSQLException(null, ex.message);
      else
        throw ex;
    }
  },

  getForeignKeys : function (pcatalog, pschema, ptable, 
  			fcatalog, fschema, ftable) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getForeignKeys(pcatalog, pschema, ptable, fcatalog, fschema, ftable);
      return new $nsSQLResultSet(this._conn, stmt);

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      if (handle.errorCode != 0)
        throw new $nsSQLException(stmt?stmt:this._conn);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        throw new $nsSQLException(null, ex.message);
      else
        throw ex;
    }
  },

  getTypeInfo : function (dataType) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getTypeInfo(dataType);
      return new $nsSQLResultSet(this._conn, stmt);

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      if (handle.errorCode != 0)
        throw new $nsSQLException(stmt?stmt:this._conn);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        throw new $nsSQLException(null, ex.message);
      else
        throw ex;
    }
  },

  getProcedures : function (catalog, schema, procedure) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getProcedures(catalog, schema, procedure);
      return new $nsSQLResultSet(this._conn, stmt);

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      if (handle.errorCode != 0)
        throw new $nsSQLException(stmt?stmt:this._conn);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        throw new $nsSQLException(null, ex.message);
      else
        throw ex;
    }
  },

  getProcedureColumns : function (catalog, schema, procedure, column) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getProcedureColumns(catalog, schema, procedure, column);
      return new $nsSQLResultSet(this._conn, stmt);

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      if (handle.errorCode != 0)
        throw new $nsSQLException(stmt?stmt:this._conn);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        throw new $nsSQLException(null, ex.message);
      else
        throw ex;
    }
  }

};


function $nsSQLTransaction(conn) {
  this._conn = conn;
}

$nsSQLTransaction.prototype = {

  _conn: null,
  _lastError: null,

  executeSql : function (sqlStatement, arguments, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();

      var arg_len  = (arguments != null && typeof(arguments)!="undefined" ? arguments.length: 0);
      if (arg_len > 0) {
        for (i=0; i < arg_len; i++)
          stmt.AddParameter(arguments[i]);
      }

      stmt.Execute(sqlStatement);

      if (callback) {
        var rs = new $nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.errorCode != 0)
        err = new $nsSQLError(handle);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        err = new $nsSQLError(null, ex.message);
      else
        err = new $nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },


  getCatalogs : function (callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getTables('%',"","","");

      if (callback) {
        var rs = new $nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.errorCode != 0)
        err = new $nsSQLError(handle);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        err = new $nsSQLError(null, ex.message);
      else
        err = new $nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },

  getTables : function (catalog, schema, table, tableType, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getTables(catalog, schema, table, tableType);

      if (callback) {
        var rs = new $nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.errorCode != 0)
        err = new $nsSQLError(handle);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        err = new $nsSQLError(null, ex.message);
      else
        err = new $nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }

  },

  getColumns : function (catalog, schema, table, column, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getColumns(catalog, schema, table, column);

      if (callback) {
        var rs = new $nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.errorCode != 0)
        err = new $nsSQLError(handle);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        err = new $nsSQLError(null, ex.message);
      else
        err = new $nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },
  
  getPrimaryKeys : function (catalog, schema, table, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getPrimaryKeys(catalog, schema, table);

      if (callback) {
        var rs = new $nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.errorCode != 0)
        err = new $nsSQLError(handle);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        err = new $nsSQLError(null, ex.message);
      else
        err = new $nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },

  getForeignKeys : function (pcatalog, pschema, ptable, 
  			fcatalog, fschema, ftable, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getForeignKeys(pcatalog, pschema, ptable, fcatalog, fschema, ftable);

      if (callback) {
        var rs = new $nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.errorCode != 0)
        err = new $nsSQLError(handle);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        err = new $nsSQLError(null, ex.message);
      else
        err = new $nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },

  getTypeInfo : function (dataType, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getTypeInfo(dataType);

      if (callback) {
        var rs = new $nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.errorCode != 0)
        err = new $nsSQLError(handle);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        err = new $nsSQLError(null, ex.message);
      else
        err = new $nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },

  getProcedures : function (catalog, schema, procedure, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getProcedures(catalog, schema, procedure);

      if (callback) {
        var rs = new $nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.errorCode != 0)
        err = new $nsSQLError(handle);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        err = new $nsSQLError(null, ex.message);
      else
        err = new $nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },

  getProcedureColumns : function (catalog, schema, procedure, column, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.CreateStatement();
      stmt.getProcedureColumns(catalog, schema, procedure, column);

      if (callback) {
        var rs = new $nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.errorCode != 0)
        err = new $nsSQLError(handle);
      else
      if (ex instanceof Error && ex.message!="$DB$") 
        err = new $nsSQLError(null, ex.message);
      else
        err = new $nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  }

};


function $nsSQLResultSet(conn, stmt) {
  this.rowsAffected = stmt.rowCount;
  this.metaData = new $nsSQLResultSetMetaData(stmt);
  this.rows = new $nsSQLResultSetRowList(stmt);
  stmt.Close();
  this.insertId = conn.lastInsertRowID;
}

$nsSQLResultSet.prototype = {

  rows: null,
  metaData: null,

  insertId: 0,
  rowsAffected: 0
};


function $nsSQLResultSetRowList(stmt) {
  this._data = [];

  var count = stmt.columnCount;
  var id = 0;
  if (count > 0) { 
     while(stmt.Fetch()) {
       var row = new $nsValue();
       for(var i=0; i < count; i++)
         row[stmt.GetColumnName(i)] = stmt.GetVariant(i);

       this._data[id] = row;
       id++;
     }
  }
  this.length = id;

}

$nsSQLResultSetRowList.prototype = {

  _data: null,

  length : 0,

  item : function(index) {
    if (index > this.length)
      return null;
    return this._data[index];
  }

};


function $nsValue() {
}



function $nsSQLResultSetMetaData(stmt) {
  this.columnCount = stmt.columnCount;

  this._columnType = [];
  this._columnName = [];
  this._isNullable = [];

  for(var i=0; i < this.columnCount; i++) {
    this._columnType[i] = stmt.GetColumnType(i);
    this._columnName[i] = stmt.GetColumnName(i);
    this._isNullable[i] = stmt.IsColumnNullable(i);
  }
}


$nsSQLResultSetMetaData.prototype = {

  _columnType: null,
  _columnName: null,
  _isNullable: null,
  
  VALUE_TYPE_UNKNOWN:  0,
  VALUE_TYPE_CHAR:     1,
  VALUE_TYPE_NUMERIC:  2,
  VALUE_TYPE_DECIMAL:  3,
  VALUE_TYPE_INTEGER:  4,
  VALUE_TYPE_SMALLINT: 5,
  VALUE_TYPE_FLOAT:    6,
  VALUE_TYPE_REAL:     7,
  VALUE_TYPE_DOUBLE:   8,
  VALUE_TYPE_DATE:     9,
  VALUE_TYPE_TIME:    10,
  VALUE_TYPE_TIMESTAMP:  11,
  VALUE_TYPE_VARCHAR:    12,
  VALUE_TYPE_intVARCHAR: -1,
  VALUE_TYPE_BINARY:     -2,
  VALUE_TYPE_VARBINARY:  -3,
  VALUE_TYPE_intVARBINARY: -4,
  VALUE_TYPE_BIGINT:   -5,
  VALUE_TYPE_TINYINT:  -6,
  VALUE_TYPE_BIT:      -7,
  VALUE_TYPE_WCHAR:    -8,
  VALUE_TYPE_WVARCHAR: -9,
  VALUE_TYPE_WLONGVARCHAR: -10,
  VALUE_TYPE_GUID: -11,

  columnCount: 0,

  getColumnType : function (index) {
    return this._columnType[index];
  },

  getColumnName : function (index) {
    return this._columnName[index];
  },


  isNullable : function(index) {
    return this._isNullable[index];
  }

};

}());

var WorkerUtils = new $nsODBC();
