angularControllers
  .controller('RendererCtrl', ['$scope', '$timeout', 'Config', function($scope, $timeout, Config) {
    var partial = 'renderer';
    UniversalController($scope, $timeout, Config, partial);
  }]);
