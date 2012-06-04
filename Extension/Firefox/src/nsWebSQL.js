/* ***** BEGIN LICENSE BLOCK *****
 * ***** END LICENSE BLOCK ***** */

var __log = Components.classes["@mozilla.org/consoleservice;1"].
                                getService(Components.interfaces.nsIConsoleService);


//can be used for DEBUG messages to jconsole
function LOG(str) {
  __log.logStringMessage(str);
}

// You can change these if you like

const CLASS_ID = Components.ID("cc1be662-adfd-4505-aed2-b41f23e5b3ee");
const CLASS_NAME = "WorkerUtils";
const CONTRACT_ID = "@openlinksw.com/WebSQLDatabase;1";

const Ci = Components.interfaces;
const Cc = Components.classes;
const Cr = Components.results;

const nsIDataType = Components.interfaces.nsIDataType;



function iODBCWebSQL() {
};

iODBCWebSQL.prototype = {
  // for nsISupports
  QueryInterface: function(aIID)
  {
    if (!aIID.equals(Ci.nsIWorkerUtilsDatabase) &&
        !aIID.equals(Ci.nsISecurityCheckedComponent) &&
        !aIID.equals(Ci.nsIClassInfo) &&
        !aIID.equals(Ci.nsISupports))
        throw Cr.NS_ERROR_NO_INTERFACE;
    return this;
  },

  // nsISecurityCheckedComponent
  canCreateWrapper : function (iid) { return "allAccess"; },
  canCallMethod : function(iid, methodName) { return "allAccess"; },
  canGetProperty : function(iid, propName) { return "allAccess"; },
  canSetProperty : function(iid, propName) { return "allAccess"; },
   
  // nsIClassInfo
  flags : Ci.nsIClassInfo.DOM_OBJECT,
  classDescription : CLASS_NAME,

  getInterfaces : function(count) {
    var interfaceList = [Ci.nsIWorkerUtilsDatabase, Ci.nsISecurityCheckedComponent, Ci.nsIClassInfo];
    count.value = interfaceList.length;
    return interfaceList;
    },

  getHelperForLanguage : function(count) {return null;},

  openDatabaseSync : function (name, version, displayName, estimateSize, creationCallback) {
    var srv = Cc["@openlinksw.com/odbc;1"].getService(Ci.nsIODBCStorageService);
    var conn = null;
    try {
      conn = srv.openDriverDatabase(name);
    } catch (ex) {
      throw new nsSQLException(srv);
    }

    var conn_ver = conn.schemaVersion;
    if (typeof(version) == "string" && version.length > 0) {
      var dbver = parseInt(version);
      if (dbver != conn_ver)
        throw "Wrong database version: "+conn_ver;
    }
    var db = (new nsDatabaseSync(conn, conn_ver)).QueryInterface(Ci.nsIDatabaseSync);
    if (creationCallback) {
      creationCallback.handleEvent(db);
    }       
    return db;
  },

  openDatabase : function (name, version, displayName, estimateSize, creationCallback) {
    var srv = Cc["@openlinksw.com/odbc;1"].getService(Ci.nsIODBCStorageService);
    var conn = null;
    try {
      conn = srv.openDriverDatabase(name);
    } catch (ex) {
      throw new nsSQLException(srv);
    }

    var conn_ver = conn.schemaVersion;
    if (typeof(version) == "string" && version.length > 0) {
      var dbver = parseInt(version);
      if (dbver != conn_ver)
        throw "Wrong database version: "+conn_ver;
    }
    var db = (new nsDatabase(conn, conn_ver)).QueryInterface(Ci.nsIDatabase);
    if (creationCallback) {
      creationCallback.handleEvent(db);
    }       
    return db;
  },


  getDataSources : function() {
    var srv = Cc["@openlinksw.com/odbc;1"].getService(Ci.nsIODBCStorageService);
    var val;
    var ret = [];
    var i = 0;

    val = srv.getDataSource(true);
    if (val != null) {
      ret[i++] = val;
      while(val != null) {
        val = srv.getDataSource(false);
        if (val != null)
          ret[i++] = val;
      }
    }
    return ret;
  },


};

//=================================================
// Note: You probably don't want to edit anything
// below this unless you know what you're doing.
//
// Factory
var iODBCWebSQLFactory = {
  singleton: null,
  createInstance: function (aOuter, aIID)
  {
    if (aOuter != null)
      throw Cr.NS_ERROR_NO_AGGREGATION;
    if (this.singleton == null)
      this.singleton = new iODBCWebSQL();
    return this.singleton.QueryInterface(aIID);
  }
};

// Module
var iODBCWebSQLModule = {
  registerSelf: function(aCompMgr, aFileSpec, aLocation, aType)
  {
    aCompMgr = aCompMgr.QueryInterface(Ci.nsIComponentRegistrar);
    aCompMgr.registerFactoryLocation(CLASS_ID, CLASS_NAME, CONTRACT_ID, aFileSpec, aLocation, aType);

    const CATMAN_CONTRACTID = "@mozilla.org/categorymanager;1";
    const nsICategoryManager = Components.interfaces.nsICategoryManager;
    var catman = Components.classes[CATMAN_CONTRACTID].
                            getService(nsICategoryManager);

    const JAVASCRIPT_GLOBAL_PROPERTY_CATEGORY = "JavaScript global property";
    catman.addCategoryEntry(JAVASCRIPT_GLOBAL_PROPERTY_CATEGORY,
                            CLASS_NAME,
                            CONTRACT_ID,
                            true,
                            true);
  },

  unregisterSelf: function(aCompMgr, aLocation, aType)
  {
    const CATMAN_CONTRACTID = "@mozilla.org/categorymanager;1";
    const nsICategoryManager = Components.interfaces.nsICategoryManager;
    var catman = Components.classes[CATMAN_CONTRACTID].
                            getService(nsICategoryManager);

    const JAVASCRIPT_GLOBAL_PROPERTY_CATEGORY = "JavaScript global property";
    catman.deleteCategoryEntry(JAVASCRIPT_GLOBAL_PROPERTY_CATEGORY,
                            CLASS_NAME,
                            true);

    aCompMgr = aCompMgr.QueryInterface(Ci.nsIComponentRegistrar);
    aCompMgr.unregisterFactoryLocation(CLASS_ID, aLocation);
  },
  
  getClassObject: function(aCompMgr, aCID, aIID)
  {
    if (!aIID.equals(Ci.nsIFactory))
      throw Components.results.NS_ERROR_NOT_IMPLEMENTED;

    if (aCID.equals(CLASS_ID))
      return iODBCWebSQLFactory;

    throw Cr.NS_ERROR_NO_INTERFACE;
  },

  canUnload: function(aCompMgr) { return true; },

};

//module initialization
function NSGetModule(aCompMgr, aFileSpec) { return iODBCWebSQLModule; };

function NSGetFactory(aCID) { 
  if (aCID.equals(CLASS_ID))
    return iODBCWebSQLFactory;

  throw Cr.NS_ERROR_NO_INTERFACE;
};



function isIID(a, iid) {
  var rv = false;
  try {
    a.QueryInterface(iid);
    rv = true;
  } catch(e) {
  }
  return rv;
}



//============================================================================
//======================== nsIDatabaseSync ===================================
//============================================================================

function nsDatabaseSync(conn, conn_ver) {
  this._conn = conn;
  this._conn_ver = conn_ver;
}


nsDatabaseSync.prototype = {

  QueryInterface: function(aIID)
  {
    if (!aIID.equals(Ci.nsIDatabaseSync) &&
        !aIID.equals(Ci.nsISecurityCheckedComponent) &&
        !aIID.equals(Ci.nsISupports))
        throw Cr.NS_ERROR_NO_INTERFACE;
    return this;
  },

  canCreateWrapper : function (iid) { return "allAccess"; },
  canCallMethod : function(iid, methodName) { return "allAccess"; },
  canGetProperty : function(iid, propName) { return "allAccess"; },
  canSetProperty : function(iid, propName) { return "allAccess"; },

  //val: "<default value>"
  _conn: null,
  _conn_ver: 0,


  get version()  { return ""+this._conn_ver; },

  transaction : function (callback) {
    if (!this._conn)
        throw "Connection isn't opened";

    if (!callback)
      throw "Transaction callback function is null";

    var msc = new nsSQLTransactionSync(this, this._conn);
    if (!msc)
       throw "Error out of Memory";

    try {
       this._conn.beginTransaction();
       this._conn.readOnlyMode = false;

       callback.handleEvent(msc);

       this._conn.commitTransaction();
    } catch (ex) {
       if (this._conn.lastErrorCode != 0)
         ex = new nsSQLException(this._conn);
       else
         ex = new nsSQLException(null, ex.toString());

       try {
         this._conn.rollbackTransaction();
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

    var msc = new nsSQLTransactionSync(this, this._conn);
    if (!msc)
       throw "Error out of Memory";

    try {
       this._conn.beginTransaction();
       this._conn.readOnlyMode = true;

       callback.handleEvent(msc);

       this._conn.commitTransaction();
    } catch (ex) {
       if (this._conn.lastErrorCode != 0)
         ex = new nsSQLException(this._conn);
       else
         ex = new nsSQLException(null, ex.toString());

       try {
         this._conn.rollbackTransaction();
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

   if (oldVer != this._conn_ver)
     throw "Database version isn't equal connection version";

     var msc = new nsSQLTransactionSync(this, this._conn);
     if (!msc)
       throw "Error out of Memory";

     try {
        this._conn.beginTransaction();

        if (callback)
          callback.handleEvent(msc);

        this._conn.schemaVersion = newVer;
        this._conn_ver = newVer;

        this._conn.commitTransaction();
     } catch (ex) {
       if (this._conn.lastErrorCode != 0)
         ex = new nsSQLException(this._conn);
       else
         ex = new nsSQLException(null, ex.toString());

        try {
          this._conn.rollbackTransaction();
        } catch (e) {}
        throw ex;
     } finally {
        delete msc;
     }
  },


};


//=========================================================================
//======================== nsIDatabase ===================================
//=========================================================================


function nsDatabase(conn, conn_ver) {
  this._conn = conn;
  this._conn_ver = conn_ver;
  this._thread = Cc["@mozilla.org/thread-manager;1"].getService(Ci.nsIThreadManager).currentThread;
}


nsDatabase.prototype = {

  QueryInterface: function(aIID)
  {
    if (!aIID.equals(Ci.nsIDatabase) &&
        !aIID.equals(Ci.nsISecurityCheckedComponent) &&
        !aIID.equals(Ci.nsISupports))
        throw Cr.NS_ERROR_NO_INTERFACE;
    return this;
  },

  canCreateWrapper : function (iid) { return "allAccess"; },
  canCallMethod : function(iid, methodName) { return "allAccess"; },
  canGetProperty : function(iid, propName) { return "allAccess"; },
  canSetProperty : function(iid, propName) { return "allAccess"; },

  //val: "<default value>"
  _conn: null,
  _conn_ver: 0,
  _thread: null,

  get version()  { return ""+this._conn_ver; },

  transaction : function (callback, errorCallback, successCallback) {
    if (!this._conn)
        throw "Connection isn't opened";

    if (!callback)
      throw "Transaction callback function is null";

    this._thread.dispatch({
      _self: this,
      run : function() {

        var msc = new nsSQLTransaction(this._self._conn);

        try {
          this._self._conn.beginTransaction();
          this._self._conn.readOnlyMode = false;

          callback.handleEvent(msc);
          if (msc._lastError)
            throw msc._lastError;

          this._self._conn.commitTransaction();

          if (successCallback)
            successCallback.handleEvent();
        
        } catch (ex) {
          var err = null;
          if (this._self._conn.lastErrorCode != 0)
            err = new nsSQLError(this._self._conn);
          else
            err = new nsSQLError(null, ex.toString());

          try {
            this._self._conn.rollbackTransaction();
          } catch (e) {}

          if (errorCallback)
            errorCallback.handleEvent(err);
        } finally {
          delete msc;
        }
      }
    }, Ci.nsIThread.DISPATCH_NORMAL);
  },


  readTransaction : function(callback, errorCallback, successCallback) {
    if (!this._conn)
        throw "Connection isn't opened";

    if (!callback)
      throw "Transaction callback function is null";

    this._thread.dispatch({
      _self: this,
      run : function() {

        var msc = new nsSQLTransaction(this._self._conn);

        try {
          this._self._conn.beginTransaction();
          this._self._conn.readOnlyMode = true;

          callback.handleEvent(msc);
          if (msc._lastError)
            throw msc._lastError;

          this._self._conn.commitTransaction();

          if (successCallback) 
            successCallback.handleEvent();

        } catch (ex) {
          var err = null;
          if (this._self._conn.lastErrorCode != 0)
            err = new nsSQLError(this._self._conn);
          else
            err = new nsSQLError(null, ex.toString());

          try {
            this._self._conn.rollbackTransaction();
          } catch (e) {}

          if (errorCallback)
            errorCallback.handleEvent(err);
        } finally {
          delete msc;
          this._self._conn.readOnlyMode = false;
        }
      }
    }, Ci.nsIThread.DISPATCH_NORMAL);
  },


  changeVersion : function (oldVersion, newVersion, callback, errorCallback, successCallback) {
    var oldVer = parseInt(oldVersion);
    var newVer = parseInt(newVersion);

    if (oldVer != this._conn_ver)
      throw "Database version isn't equal connection version";

    this._thread.dispatch({
      _self: this,
      run : function() {
        var msc = new nsSQLTransaction(this._self._conn);

        try {
          this._self._conn.beginTransaction();

          if (callback)
            callback.handleEvent(msc);
          if (msc._lastError)
            throw msc._lastError;

          this._self._conn.schemaVersion = newVer;
          this._self._conn_ver = newVer;

          this._self._conn.commitTransaction();

          if (successCallback)
            successCallback.handleEvent();
          
        } catch (ex) {
          var err = null;
          if (this._self._conn.lastErrorCode != 0)
            err = new nsSQLError(this._self._conn);
          else
            err = new nsSQLError(null, ex.toString());

          try {
            this._self._conn.rollbackTransaction();
          } catch (e) {}

          if (errorCallback)
            errorCallback.handleEvent(err);
        } finally {
          delete msc;
        }
      }
    }, Ci.nsIThread.DISPATCH_NORMAL);
  },


};


//============================================================================
//======================== nsISQLTransactionSync ===================================
//============================================================================

function nsSQLTransactionSync(tran, conn) {
  this._tran = tran;
  this._conn = conn;
}

nsSQLTransactionSync.prototype = {

  QueryInterface: function(aIID)
  {
    if (!aIID.equals(Ci.nsISQLTransactionSync) &&
        !aIID.equals(Ci.nsISecurityCheckedComponent) &&
        !aIID.equals(Ci.nsISupports))
        throw Cr.NS_ERROR_NO_INTERFACE;
    return this;
  },

  canCreateWrapper : function (iid) { return "allAccess"; },
  canCallMethod : function(iid, methodName) { return "allAccess"; },
  canGetProperty : function(iid, propName) { return "allAccess"; },
  canSetProperty : function(iid, propName) { return "allAccess"; },

  _conn: null,
  _tran: null,

  executeSql : function (sqlStatement, arguments) {
    var stmt = null;
    try {
      stmt = this._conn.createStatement(sqlStatement);

      if (arguments) {
        for(var i=0; i < arguments.length; i++) {
          stmt.bindParameter(i, arguments[i]);
        }
      }

      stmt.executeSql();

      var rs = new nsSQLResultSet(this._conn, stmt);
      return rs.QueryInterface(Ci.nsISQLResultSet);

    } catch (ex) {
      throw new nsSQLException(stmt?stmt:this._conn);
    }
  },

  executeSql2 : function (sqlStatement, params) {
    var stmt = null;
    try {
      stmt = this._conn.createStatement(sqlStatement);

      if (params) {
        for(var i=0; i < params.length; i++) {
          switch(params.dtype[i]) {
            case nsIDataType.VTYPE_INT16:
              stmt.bindInt16Parameter(i, params.data[i]);
              break;
            case nsIDataType.VTYPE_INT32:
              stmt.bindInt32Parameter(i, params.data[i]);
              break;
            case nsIDataType.VTYPE_INT64:
              stmt.bindInt64Parameter(i, params.data[i]);
              break;
            case nsIDataType.VTYPE_FLOAT:
              stmt.bindFloatParameter(i, params.data[i]);
              break;
            case nsIDataType.VTYPE_DOUBLE:
              stmt.bindDoubleParameter(i, params.data[i]);
              break;
            case nsIDataType.VTYPE_ASTRING:
              if (params.islong[i])
                stmt.bindLongStringParameter(i, params.data[i]);
              else
                stmt.bindStringParameter(i, params.data[i]);
              break;
            case nsIDataType.VTYPE_ARRAY:
              if (params.islong[i])
                stmt.bindBlobParameter(i, params.data[i], params.data[i].length);
              else
                stmt.bindBinaryParameter(i, params.data[i], params.data[i].length);
              break;
            case -1:
              stmt.bindParameter(i, params.data[i]);
              break;
            default:
              stmt.bindNullParameter(i);
          }
        }
      }

      stmt.executeSql();

      var rs = new nsSQLResultSet(this._conn, stmt);
      return rs.QueryInterface(Ci.nsISQLResultSet);

    } catch (ex) {
      throw new nsSQLException(stmt?stmt:this._conn);
    }
  },

  createParameterArray : function () {
    var ret = new nsSQLParameterArray();
    return ret.QueryInterface(Ci.nsISQLParameterArray);
  },

  getCatalogs : function () {
    var stmt = null;
    try {
      stmt = this._conn.getTables('%',"","","");
      var rs = new nsSQLResultSet(this._conn, stmt);
      return rs.QueryInterface(Ci.nsISQLResultSet);

    } catch (ex) {
      throw new nsSQLException(stmt?stmt:this._conn);
    }
  },

  getTables : function (catalog, schema, table, tableType) {
    var stmt = null;
    try {
      stmt = this._conn.getTables(catalog, schema, table, tableType);
      var rs = new nsSQLResultSet(this._conn, stmt);
      return rs.QueryInterface(Ci.nsISQLResultSet);

    } catch (ex) {
      throw new nsSQLException(stmt?stmt:this._conn);
    }
  },

  getColumns : function (catalog, schema, table, column) {
    var stmt = null;
    try {
      stmt = this._conn.getColumns(catalog, schema, table, column);
      var rs = new nsSQLResultSet(this._conn, stmt);
      return rs.QueryInterface(Ci.nsISQLResultSet);

    } catch (ex) {
      throw new nsSQLException(stmt?stmt:this._conn);
    }
  },
  
  getPrimaryKeys : function (catalog, schema, table) {
    var stmt = null;
    try {
      stmt = this._conn.getPrimaryKeys(catalog, schema, table);
      var rs = new nsSQLResultSet(this._conn, stmt);
      return rs.QueryInterface(Ci.nsISQLResultSet);

    } catch (ex) {
      throw new nsSQLException(stmt?stmt:this._conn);
    }
  },

  getForeignKeys : function (pcatalog, pschema, ptable, 
  			fcatalog, fschema, ftable) {
    var stmt = null;
    try {
      stmt = this._conn.getForeignKeys(pcatalog, pschema, ptable, fcatalog, fschema, ftable);
      var rs = new nsSQLResultSet(this._conn, stmt);
      return rs.QueryInterface(Ci.nsISQLResultSet);

    } catch (ex) {
      throw new nsSQLException(stmt?stmt:this._conn);
    }
  },

  getTypeInfo : function (dataType) {
    var stmt = null;
    try {
      stmt = this._conn.getTypeInfo(dataType);
      var rs = new nsSQLResultSet(this._conn, stmt);
      return rs.QueryInterface(Ci.nsISQLResultSet);

    } catch (ex) {
      throw new nsSQLException(stmt?stmt:this._conn);
    }
  },

  getProcedures : function (catalog, schema, procedure) {
    var stmt = null;
    try {
      stmt = this._conn.getProcedures(catalog, schema, procedure);
      var rs = new nsSQLResultSet(this._conn, stmt);
      return rs.QueryInterface(Ci.nsISQLResultSet);

    } catch (ex) {
      throw new nsSQLException(stmt?stmt:this._conn);
    }
  },

  getProcedureColumns : function (catalog, schema, procedure, column) {
    var stmt = null;
    try {
      stmt = this._conn.getProcedureColumns(catalog, schema, procedure, column);
      var rs = new nsSQLResultSet(this._conn, stmt);
      return rs.QueryInterface(Ci.nsISQLResultSet);

    } catch (ex) {
      throw new nsSQLException(stmt?stmt:this._conn);
    }
  },

};


//============================================================================
//======================== nsISQLTransaction ===================================
//============================================================================

function nsSQLTransaction(conn) {
  this._conn = conn;
}

nsSQLTransaction.prototype = {

  QueryInterface: function(aIID)
  {
    if (!aIID.equals(Ci.nsISQLTransaction) &&
        !aIID.equals(Ci.nsISecurityCheckedComponent) &&
        !aIID.equals(Ci.nsISupports))
        throw Cr.NS_ERROR_NO_INTERFACE;
    return this;
  },

  canCreateWrapper : function (iid) { return "allAccess"; },
  canCallMethod : function(iid, methodName) { return "allAccess"; },
  canGetProperty : function(iid, propName) { return "allAccess"; },
  canSetProperty : function(iid, propName) { return "allAccess"; },

  _conn: null,
  _lastError: null,

  executeSql : function (sqlStatement, arguments, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.createStatement(sqlStatement);

      if (arguments) {
        for(var i=0; i < arguments.length; i++) {
          stmt.bindParameter(i, arguments[i]);
        }
      }

      stmt.executeSql();

      if (callback) {
        var rs = new nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.lastErrorCode != 0)
        err = new nsSQLError(handle);
      else
        err = new nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },

  executeSql2 : function (sqlStatement, params, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.createStatement(sqlStatement);

      if (params) {
        for(var i=0; i < params.length; i++) {
          switch(params.dtype[i]) {
            case nsIDataType.VTYPE_INT16:
              stmt.bindInt16Parameter(i, params.data[i]);
              break;
            case nsIDataType.VTYPE_INT32:
              stmt.bindInt32Parameter(i, params.data[i]);
              break;
            case nsIDataType.VTYPE_INT64:
              stmt.bindInt64Parameter(i, params.data[i]);
              break;
            case nsIDataType.VTYPE_FLOAT:
              stmt.bindFloatParameter(i, params.data[i]);
              break;
            case nsIDataType.VTYPE_DOUBLE:
              stmt.bindDoubleParameter(i, params.data[i]);
              break;
            case nsIDataType.VTYPE_ASTRING:
              if (params.islong[i])
                stmt.bindLongStringParameter(i, params.data[i]);
              else
                stmt.bindStringParameter(i, params.data[i]);
              break;
            case nsIDataType.VTYPE_ARRAY:
              if (params.islong[i])
                stmt.bindBlobParameter(i, params.data[i], params.data[i].length);
              else
                stmt.bindBinaryParameter(i, params.data[i], params.data[i].length);
              break;
            case -1:
              stmt.bindParameter(i, params.data[i]);
              break;
            default:
              stmt.bindNullParameter(i);
          }
        }
      }

      stmt.executeSql();

      if (callback) {
        var rs = new nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch(ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.lastErrorCode != 0)
        err = new nsSQLError(handle);
      else
        err = new nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  
  },

  createParameterArray : function () {
    var ret = new nsSQLParameterArray();
    return ret.QueryInterface(Ci.nsISQLParameterArray);
  },


  getCatalogs : function (callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.getTables('%',"","","");

      if (callback) {
        var rs = new nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.lastErrorCode != 0)
        err = new nsSQLError(handle);
      else
        err = new nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },

  getTables : function (catalog, schema, table, tableType, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.getTables(catalog, schema, table, tableType);

      if (callback) {
        var rs = new nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.lastErrorCode != 0)
        err = new nsSQLError(handle);
      else
        err = new nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }

  },

  getColumns : function (catalog, schema, table, column, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.getColumns(catalog, schema, table, column);

      if (callback) {
        var rs = new nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.lastErrorCode != 0)
        err = new nsSQLError(handle);
      else
        err = new nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },
  
  getPrimaryKeys : function (catalog, schema, table, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.getPrimaryKeys(catalog, schema, table);

      if (callback) {
        var rs = new nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.lastErrorCode != 0)
        err = new nsSQLError(handle);
      else
        err = new nsSQLError(null, ex.toString());

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
      stmt = this._conn.getForeignKeys(pcatalog, pschema, ptable, fcatalog, fschema, ftable);

      if (callback) {
        var rs = new nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.lastErrorCode != 0)
        err = new nsSQLError(handle);
      else
        err = new nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },

  getTypeInfo : function (dataType, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.getTypeInfo(dataType);

      if (callback) {
        var rs = new nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.lastErrorCode != 0)
        err = new nsSQLError(handle);
      else
        err = new nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },

  getProcedures : function (catalog, schema, procedure, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.getProcedures(catalog, schema, procedure);

      if (callback) {
        var rs = new nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.lastErrorCode != 0)
        err = new nsSQLError(handle);
      else
        err = new nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },

  getProcedureColumns : function (catalog, schema, procedure, column, callback, errorCallback) {
    var stmt = null;
    try {
      stmt = this._conn.getProcedureColumns(catalog, schema, procedure, column);

      if (callback) {
        var rs = new nsSQLResultSet(this._conn, stmt);
        callback.handleEvent(this, rs);
      }

    } catch (ex) {
      var handle = stmt?stmt:this._conn;
      var ret = true;
      var err;
      if (handle.lastErrorCode != 0)
        err = new nsSQLError(handle);
      else
        err = new nsSQLError(null, ex.toString());

      if (errorCallback)
        ret = errorCallback.handleEvent(this, err);
      if (ret)
        this._lastError = err;
    }
  },


};


//============================================================================
//======================== nsISQLError ===================================
//============================================================================

function nsSQLError(handle, message) {
  if (handle != null) {
    this._code = handle.lastErrorCode;
    this._message = handle.lastErrorString;
    this._state = handle.lastErrorState;
  } else {
    this._code = -1;
    this._message = message;
    this._state = "HY000";
  }
}

nsSQLError.prototype = {

  QueryInterface: function(aIID)
  {
    if (!aIID.equals(Ci.nsISQLError) &&
        !aIID.equals(Ci.nsISecurityCheckedComponent) &&
        !aIID.equals(Ci.nsISupports))
        throw Cr.NS_ERROR_NO_INTERFACE;
    return this;
  },

  canCreateWrapper : function (iid) { return "allAccess"; },
  canCallMethod : function(iid, methodName) { return "allAccess"; },
  canGetProperty : function(iid, propName) { return "allAccess"; },
  canSetProperty : function(iid, propName) { return "allAccess"; },

  _code: 0,
  _message: null,
  _state: null,

  get code()     { return this._code; },
  get message()  { return this._message; },
  get state()    { return this._state; },

  toString : function() {
    return this._message;
  },
};


//============================================================================
//======================== nsISQLException ===================================
//============================================================================

function nsSQLException(handle, message) {
  if (handle != null) {
    this._code = handle.lastErrorCode;
    this._message = handle.lastErrorString;
    this._state = handle.lastErrorState;
  } else {
    this._code = -1;
    this._message = message;
    this._state = "HY000";
  }
}

nsSQLException.prototype = {

  QueryInterface: function(aIID)
  {
    if (!aIID.equals(Ci.nsISQLException) &&
        !aIID.equals(Ci.nsISecurityCheckedComponent) &&
        !aIID.equals(Ci.nsISupports))
        throw Cr.NS_ERROR_NO_INTERFACE;
    return this;
  },

  canCreateWrapper : function (iid) { return "allAccess"; },
  canCallMethod : function(iid, methodName) { return "allAccess"; },
  canGetProperty : function(iid, propName) { return "allAccess"; },
  canSetProperty : function(iid, propName) { return "allAccess"; },

  _code: 0,
  _message: null,
  _state: null,

  get code()     { return this._code; },
  get message()  { return this._message; },
  get state()    { return this._state; },

  toString : function() {
    return this._message;
  },
};


//============================================================================
//======================== nsISQLResultSet ===================================
//============================================================================

function nsSQLResultSet(conn, stmt) {
  this._rowsAffected = stmt.rowCount;
  this._meta = (new nsSQLResultSetMetaData(stmt)).QueryInterface(Ci.nsISQLResultSetMetaData);
  this._rows = (new nsSQLResultSetRowList(stmt)).QueryInterface(Ci.nsISQLResultSetRowList);
  stmt.reset();
  this._insertId = conn.lastInsertRowID;
}

nsSQLResultSet.prototype = {

  QueryInterface: function(aIID)
  {
    if (!aIID.equals(Ci.nsISQLResultSet) &&
        !aIID.equals(Ci.nsISecurityCheckedComponent) &&
        !aIID.equals(Ci.nsISupports))
        throw Cr.NS_ERROR_NO_INTERFACE;
    return this;
  },

  canCreateWrapper : function (iid) { return "allAccess"; },
  canCallMethod : function(iid, methodName) { return "allAccess"; },
  canGetProperty : function(iid, propName) { return "allAccess"; },
  canSetProperty : function(iid, propName) { return "allAccess"; },

  _rows: null,
  _meta: null,
  _insertId: 0,
  _rowsAffected: 0,

  get insertId()       { return this._insertId; },
  get rowsAffected()   { return this._rowsAffected; },
  get rows()           { return this._rows;  },
  get metaData()       { return this._meta;  },

};


//============================================================================
//======================== nsISQLResultSetRowList ===================================
//============================================================================

function nsSQLResultSetRowList(stmt) {
  this._data = [];

  var count = stmt.columnCount;
  var id = 0;
  if (count > 0) { 
    while(stmt.fetch()) {
      var row = new nsValue();
      for(var i=0; i < count; i++)
        row[stmt.getColumnName(i)] = stmt.getVariant(i);

      this._data[id] = row;
      id++;
    }
  }
  this._length = id;
}

nsSQLResultSetRowList.prototype = {

  QueryInterface: function(aIID)
  {
    if (!aIID.equals(Ci.nsISQLResultSetRowList) &&
        !aIID.equals(Ci.nsISecurityCheckedComponent) &&
        !aIID.equals(Ci.nsISupports))
        throw Cr.NS_ERROR_NO_INTERFACE;
    return this;
  },

  canCreateWrapper : function (iid) { return "allAccess"; },
  canCallMethod : function(iid, methodName) { return "allAccess"; },
  canGetProperty : function(iid, propName) { return "allAccess"; },
  canSetProperty : function(iid, propName) { return "allAccess"; },

  _length : 0,
  _data: null,

  get length()       { return this._length; },

  item : function(index) {
    if (index > this._length)
      return null;
    return this._data[index];
  },

};


function nsValue() {
}


//============================================================================
//======================== nsISQLParameterArray ==============================
//============================================================================


function nsSQLParameterArray() {
  this._data = [];
  this._islong = [];
  this._dtype = [];
}

nsSQLParameterArray.prototype = {

  QueryInterface: function(aIID)
  {
    if (!aIID.equals(Ci.nsISQLParameterArray) &&
        !aIID.equals(Ci.nsISecurityCheckedComponent) &&
        !aIID.equals(Ci.nsISupports))
        throw Cr.NS_ERROR_NO_INTERFACE;
    return this;
  },

  canCreateWrapper : function (iid) { return "allAccess"; },
  canCallMethod : function(iid, methodName) { return "allAccess"; },
  canGetProperty : function(iid, propName) { return "allAccess"; },
  canSetProperty : function(iid, propName) { return "allAccess"; },

  _length : 0,
  _data: null,
  _islong: null,
  _dtype: null,

  get length()   { return this._data.length; },
  get data()     { return this._data; },
  get islong()   { return this._islong; },
  get dtype()    { return this._dtype; },


  setData : function(index, val) {
    this._data[index] = val;
    this._islong[index] = false;
    this._dtype[index] = -1;
  },

  setString : function(index, val) {
    this._data[index] = val;
    this._islong[index] = false;
    this._dtype[index] = nsIDataType.VTYPE_ASTRING;
  },

  setLongString : function(index, val) {
    this._data[index] = val;
    this._islong[index] = true;
    this._dtype[index] = nsIDataType.VTYPE_ASTRING;
  },

  setFloat : function(index, val) {
    this._data[index] = val;
    this._islong[index] = false;
    this._dtype[index] = nsIDataType.VTYPE_FLOAT;
  },

  setDouble : function(index, val) {
    this._data[index] = val;
    this._islong[index] = false;
    this._dtype[index] = nsIDataType.VTYPE_DOUBLE;
  },

  setInt16 : function(index, val) {
    this._data[index] = val;
    this._islong[index] = false;
    this._dtype[index] = nsIDataType.VTYPE_INT16;
  },

  setInt32 : function(index, val) {
    this._data[index] = val;
    this._islong[index] = false;
    this._dtype[index] = nsIDataType.VTYPE_INT32;
  },

  setInt64 : function(index, val) {
    this._data[index] = val;
    this._islong[index] = false;
    this._dtype[index] = nsIDataType.VTYPE_INT64;
  },

  setNull : function(index) {
    this._data[index] = null;
    this._islong[index] = false;
    this._dtype[index] = nsIDataType.VTYPE_EMPTY;
  },

  setBlob : function(index, val) {
    this._data[index] = val;
    this._islong[index] = true;
    this._dtype[index] = nsIDataType.VTYPE_ARRAY;
  },

  setBinary : function(index, val) {
    this._data[index] = val;
    this._islong[index] = false;
    this._dtype[index] = nsIDataType.VTYPE_ARRAY;
  },

};


//============================================================================
//======================== nsISQLResultSetMetaData ===========================
//============================================================================

function nsSQLResultSetMetaData(stmt) {
  this._count = stmt.numEntries;

  this._columnType = [];
  this._columnName = [];
  this._isNullable = [];

  for(var i=0; i < this._count; i++) {
    this._columnType[i] = stmt.getTypeOfIndex(i);
    this._columnName[i] = stmt.getColumnName(i);
    this._isNullable[i] = stmt.isNullable(i);
  }
}


nsSQLResultSetMetaData.prototype = {

  QueryInterface: function(aIID)
  {
    if (!aIID.equals(Ci.nsISQLResultSetMetaData) &&
        !aIID.equals(Ci.nsISecurityCheckedComponent) &&
        !aIID.equals(Ci.nsISupports))
        throw Cr.NS_ERROR_NO_INTERFACE;
    return this;
  },

  canCreateWrapper : function (iid) { return "allAccess"; },
  canCallMethod : function(iid, methodName) { return "allAccess"; },
  canGetProperty : function(iid, propName) { return "allAccess"; },
  canSetProperty : function(iid, propName) { return "allAccess"; },

  _count: 0,
  _columnType: null,
  _columnName: null,
  _isNullable: null,

  get columnCount()  { return this._count; },

  getColumnType : function (index) {
    return this._columnType[index];
  },

  getColumnName : function (index) {
    return this._columnName[index];
  },

  isNullable : function(index) {
    return this._isNullable[index];
  },

};


