'use strict';

// Declare app level module which depends on filters, and services
angular.module('configApp', [
  'ngRoute',
  'configApp.filters',
  'configApp.services',
  'configApp.directives',
  'configApp.controllers',
]).
config(['$routeProvider', function($routeProvider) {
  $routeProvider.when('/about', {templateUrl: 'partials/about.html', controller: 'AboutCtrl'});
  $routeProvider.when('/device', {templateUrl: 'partials/device.html', controller: 'DeviceCtrl'});
  $routeProvider.when('/volume', {templateUrl: 'partials/volume.html', controller: 'VolumeCtrl'});
  $routeProvider.when('/renderer', {templateUrl: 'partials/renderer.html', controller: 'RendererCtrl'});
  $routeProvider.when('/preamp', {templateUrl: 'partials/preamp.html', controller: 'PreampCtrl'});
  $routeProvider.otherwise({redirectTo: '/about'});
}]);
