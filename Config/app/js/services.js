'use strict';

/* Services */

angular.module('configApp.services', ['ngResource'])
  .factory('Config', ['$resource',
    function ($resource){
      return $resource('data/:partial.json', {}, {
        query: {method: 'GET'}
      });
  }]);
