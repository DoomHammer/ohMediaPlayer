'use strict';

/* Controllers */

var UniversalController = function($scope, $timeout, Config, partial) {
    $scope.data = Config.get({"partial": partial});

    var timeout = null;
    var saveInProgress = false;
    var saveFinished = function() { saveInProgress = false; };

    var saveFunction = function() {
      $scope.data.$save({"partial": partial});
    };

    var saveUpdates = function() {
      if ($scope[partial].$valid) {
        saveInProgress = true;
        try {
          saveFunction();
        }
        finally {
          saveFinished();
        }
      }
    };

    var debounceSaveUpdates = function(newVal, oldVal) {
      if (newVal != oldVal) {
        if (timeout) {
          $timeout.cancel(timeout)
        }
        var nihi = function(){};
        timeout = $timeout(nihi, 1000);
        timeout.then(function(result){saveUpdates($scope, partial, saveInProgress, saveFinished)});
      }
    };

    $scope.data.$promise.then(function(result){
      angular.forEach(Object.keys($scope.data), function(value, key) {
        if (value[0] != '$')
        {
          this.$watch('data.' + value, debounceSaveUpdates);
        }
      }, $scope);
    });
};

angular.module('configApp.controllers', [])
  .controller('AboutCtrl', ['$scope', 'Config', function($scope, Config) {
    var partial = "about";
    $scope.data = Config.get({"partial": partial});
  }])
  .controller('DeviceCtrl', ['$scope', '$timeout', 'Config', function($scope, $timeout, Config) {
    var partial = "device";
    UniversalController($scope, $timeout, Config, partial);
  }])
  .controller('VolumeCtrl', ['$scope', '$timeout', 'Config', function($scope, $timeout, Config) {
    var partial = "volume";
    UniversalController($scope, $timeout, Config, partial);
  }])
  .controller('MenuCtrl', ['$scope', '$location', function($scope, $location) {
    $scope.isActive = function (viewLocation) {
      var active = (viewLocation === $location.path());
      return active;
    };
  }]);
