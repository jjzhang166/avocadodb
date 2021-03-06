/*jshint strict: false */

// //////////////////////////////////////////////////////////////////////////////
// / @brief AvocadoView
// /
// / @file
// /
// / DISCLAIMER
// /
// / Copyright 2013 triagens GmbH, Cologne, Germany
// /
// / Licensed under the Apache License, Version 2.0 (the "License")
// / you may not use this file except in compliance with the License.
// / You may obtain a copy of the License at
// /
// /     http://www.apache.org/licenses/LICENSE-2.0
// /
// / Unless required by applicable law or agreed to in writing, software
// / distributed under the License is distributed on an "AS IS" BASIS,
// / WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// / See the License for the specific language governing permissions and
// / limitations under the License.
// /
// / Copyright holder is triAGENS GmbH, Cologne, Germany
// /
// / @author Daniel H. Larkin
// / @author Copyright 2012-2013, triAGENS GmbH, Cologne, Germany
// //////////////////////////////////////////////////////////////////////////////

var internal = require('internal');
var avocadosh = require('@avocadodb/avocadosh');

// //////////////////////////////////////////////////////////////////////////////
// / @brief constructor
// //////////////////////////////////////////////////////////////////////////////

function AvocadoView (database, data) {
  this._database = database;
  this._dbName = database._name();
  this._dbPrefix = '/_db/' + encodeURIComponent(database._name());

  if (typeof data === 'string') {
    this._id = null;
    this._name = data;
    this._type = null;
  }
  else if (data !== undefined) {
    this._id = data.id;
    this._name = data.name;
    this._type = data.type;
  } else {
    this._id = null;
    this._name = null;
    this._type = null;
  }
}

exports.AvocadoView = AvocadoView;

var AvocadoError = require('@avocadodb').AvocadoError;

// //////////////////////////////////////////////////////////////////////////////
// / @brief append the waitForSync parameter to a URL
// //////////////////////////////////////////////////////////////////////////////

AvocadoView.prototype._appendSyncParameter = function (url, waitForSync) {
  if (waitForSync) {
    if (url.indexOf('?') === -1) {
      url += '?';
    }else {
      url += '&';
    }
    url += 'waitForSync=true';
  }
  return url;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief append some boolean parameter to a URL
// //////////////////////////////////////////////////////////////////////////////

AvocadoView.prototype._appendBoolParameter = function (url, name, val) {
  if (url.indexOf('?') === -1) {
    url += '?';
  }else {
    url += '&';
  }
  url += name + (val ? '=true' : '=false');
  return url;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief prefix a URL with the database name of the view
// //////////////////////////////////////////////////////////////////////////////

AvocadoView.prototype._prefixurl = function (url) {
  if (url.substr(0, 5) === '/_db/') {
    return url;
  }

  if (url[0] === '/') {
    return this._dbPrefix + url;
  }
  return this._dbPrefix + '/' + url;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief return the base url for view usage
// //////////////////////////////////////////////////////////////////////////////

AvocadoView.prototype._baseurl = function (suffix) {
  var url = this._database._viewurl(this.name());

  if (suffix) {
    url += '/' + suffix;
  }

  return this._prefixurl(url);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief converts into an array
// //////////////////////////////////////////////////////////////////////////////

AvocadoView.prototype.toArray = function () {
  return this.all().toArray();
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief print the help for AvocadoView
// //////////////////////////////////////////////////////////////////////////////

var helpAvocadoView = avocadosh.createHelpHeadline('AvocadoView help') +
  'AvocadoView constructor:                                             ' + '\n' +
  ' > view = db._view("myview");                                       ' + '\n' +
  ' > col = db._createView("myview", "type", {[properties]});          ' + '\n' +
  '                                                                          ' + '\n' +
  'Administration Functions:                                                 ' + '\n' +
  '  name()                                view name                   ' + '\n' +
  '  type()                                type of the view            ' + '\n' +
  '  properties()                          show view properties        ' + '\n' +
  '  drop()                                delete a view               ' + '\n' +
  '  _help()                               this help                         ' + '\n' +
  '                                                                          ' + '\n' +
  'Attributes:                                                               ' + '\n' +
  '  _database                             database object                   ' + '\n' +
  '  _id                                   view identifier             ';

AvocadoView.prototype._help = function () {
  internal.print(helpAvocadoView);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief gets the name of a view
// //////////////////////////////////////////////////////////////////////////////

AvocadoView.prototype.name = function () {
  if (this._name === null) {
    this.refresh();
  }

  return this._name;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief gets the type of a view
// //////////////////////////////////////////////////////////////////////////////

AvocadoView.prototype.type = function () {
  if (this._type === null) {
    this.refresh();
  }

  return this._type;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief gets or sets the properties of a view
// //////////////////////////////////////////////////////////////////////////////

AvocadoView.prototype.properties = function (properties) {
  var requestResult;
  if (properties === undefined) {
    requestResult = this._database._connection.GET(this._baseurl('properties'));

    avocadosh.checkRequestResult(requestResult);
  } else {
    var body = properties;
    requestResult = this._database._connection.PATCH(this._baseurl('properties'),
      JSON.stringify(body));

    avocadosh.checkRequestResult(requestResult);
  }

  return requestResult;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief drops a view
// //////////////////////////////////////////////////////////////////////////////

AvocadoView.prototype.drop = function () {
  var requestResult = this._database._connection.DELETE(this._baseurl());

  if (requestResult !== null
    && requestResult.error === true
    && requestResult.errorNum !== internal.errors.ERROR_AVOCADO_VIEW_NOT_FOUND.code) {
    // check error in case we got anything else but "view not found"
    avocadosh.checkRequestResult(requestResult);
  } else {
    this._database._unregisterView(this._name);
  }
};
