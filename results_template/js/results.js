(function (angular) {
  
  'use strict';
  
  var accvv = angular.module('accvv', ['ngRoute','ngAnimate', 'ngSanitize', 'ui.bootstrap', 'ansiToHtml']);
  //const fs = require('fs');
  //var textJson;
  //fs.readFile('./results.json', (err, textJson));
  //var jsonResults = JSON.parse(textJson);
  function getCompilerName(configID, testName){
	  var ReturnedString = "";
	  var configRef;
	  for (var conf = 0; conf < jsonResults["configs"].length; conf++){
		 if (jsonResults["configs"][conf]["id"] == configID){
           configRef = jsonResults["configs"][conf];
         }		   
      }
	  if (testName.endsWith(".F90") || testName.endsWith(".f90")){
		  ReturnedString = configRef["FC"];
      }
	  else{
		ReturnedString = configRef["CC"];
	  }
	  return ReturnedString;
  }
  
  function getConfigName(configID){
	  for (var conf = 0; conf < jsonResults["configs"].length; conf++){
		  if (jsonResults["configs"][conf]["id"] == configID){
			  return jsonResults["configs"][conf]["config_name"];
		  }
	  }
  }
  
  function getSystemName(systemID){
		var ReturnedString = "";
		var systemRef;
		for (var system = 0; system < jsonResults["systems"].length; ++system){
			if (jsonResults["systems"][system]["id"] == systemID){
				systemRef = jsonResults["systems"][system];
			}
		}
		for (var index in systemRef){
			ReturnedString = ReturnedString + index + " : " + systemRef[index] + "\t";
		}
		return ReturnedString;
  }
  
  function getNumTests(testname, given_id){
		var TestsuiteRef;
		for (var testsuite_id = 0; testsuite_id < jsonResults["testsuites"].length; ++testsuite_id){
			if (jsonResults["testsuites"][testsuite_id]["id"] == given_id){
				TestsuiteRef = testsuite_id;
			}
		}
		return jsonResults["testsuites"][TestsuiteRef][testname]["num tests"];
  }
  
  function getResultString(resultNum){
		if (resultNum == 0){
			return "PASS";
		}
		else{
			return "FAIL";
		}
  }
  function getSegmentContents(testsuite_id, testname, segment_num){
	 var TestsuiteRef;
	 for (var ts_id = 0; ts_id < jsonResults["testsuites"].length; ++ts_id){
		 if (jsonResults["testsuites"][ts_id]["id"] == testsuite_id){
			 TestsuiteRef = ts_id;
		 }
	 }
	 return jsonResults["testsuites"][TestsuiteRef][testname]["tests"][segment_num]["content"];
  }
  function getResultString2(resultNum, testName, commandString, testsuite_id){
	  var numTests = getNumTests(testName, testsuite_id);
	  var skippedTests = {}
	  for (var x = 0; x < numTests; ++x){
		  skippedTests[x + 1] = false;
	  }
	  var flags = commandString.split(" ");
	  for (var x = 0; x < flags.length; ++x){
		  if (flags[x].startsWith("-DT")){
			  skippedTests[parseInt(flags[x][3], 10) + 1] = true;
			  console.log("Skipped Test " + (parseInt(flags[x][3], 10) + 1));
		  }
	  }
	  var returned = {};
	  returned["compilation"] = "";
	  returned["runtime"] = "";
	  var tempResult = resultNum;
	  for (var x = 0; x < numTests; ++x){
		  if (skippedTests[x + 1]){
			  returned["compilation"] = returned["compilation"] + '?';
		      returned["runtime"] = returned["runtime"] + "?";
			  tempResult = tempResult / 2;
			  console.log("In here");
		  }
		  else if(resultNum == -1){
			  returned["compilation"] = returned["compilation"] + "F";
			  returned["runtime"] = returned["runtime"] + "?";
		  }
		  else{
			  returned["compilation"] = returned["compilation"] + "P";
			  if ((tempResult % 2) == 1){
				  returned["runtime"] = returned["runtime"] + "F";
			  }
			  else{
				  returned["runtime"] = returned["runtime"] + "P";
			  }
			  tempResult = tempResult / 2;
		  }
	  }
	  return returned;
  }
  accvv.controller('accvvController', 
  ['$scope', '$http', '$log','$uibModal', '$timeout', 'ansi2html', '$filter',
    function($scope, $http, $log, $uibModal, $timeout, ansi2html, $filter) {

      // Info message regarding table
      $scope.displayMessage = true;
      $timeout(function(){
        $scope.displayMessage = false;
      }, 5000)

      // var to show and hide results loading
      $scope.loadingResults = true;

      // Var to show and hide error message if no results were found
      $scope.errorMessage = false;

      // Table variables and getting the json file from the server
      $scope.tableContent = [];

      // Function to load results
      $scope.loadResults = function(){
        if (typeof jsonResults == 'undefined'){
          $scope.loadingResults = false;
          $scope.errorMessage = true;
          $log.log("Error loading results json file");
          $scope.error = "Error loading results json file";
          return;
        }
        $scope.tableContent = [];
		var counter = 0;
		angular.forEach(jsonResults["summary"], function(config_dict, config_id){
			angular.forEach(config_dict, function(system_dict, system_id){
				angular.forEach(system_dict, function(testsuite_dict, testsuite_id){
					angular.forEach(testsuite_dict, function(testname_dict, testname){
					  var config_ref;
					  for (var conf = 0; conf < jsonResults["configs"].length; conf++){
						 if (jsonResults["configs"][conf]["id"] == config_id){
						   config_ref = jsonResults["configs"][conf];
						 }		   
					  }
					  
					  var system_ref;
					  for (var sys = 0; sys < jsonResults["systems"].length; sys++){
						  if (jsonResults["systems"][sys]["id"] == system_id){
							  system_ref = jsonResults["systems"][sys];
						  }
					  }
					  
					  var testsuite_ref;
					  for (var ts = 0; ts < jsonResults["testsuites"].length; ts++){
						  if (jsonResults["testsuites"][ts]["id"] == testsuite_id){
							  testsuite_ref = jsonResults["testsuites"][ts];
						  }
					  }
					  
					  var compilerName;
					  /*if (testname.endsWith(".F90") || testname.endsWith(".f90")){
						  compilerName = config_ref["FC"];
					  }
					  else{
						  compilerName = config_ref["CC"];
					  }*/
					  compilerName = getConfigName(config_id);
					  
					  var testSystem = system_ref["processor"] + ", " + system_ref["node"];
					  if ("name" in system_ref){
						  if (system_ref["name"] != null){
							  testSystem = system_ref["name"];
						  }
					  }
					  
					  				  
					  var resultsString = "";
					  var compilerResult = "Pass";
					  var runtimeResult = "Pass";
					  var compilationResultSet = []
					  var runtimeResultSet = []
					  var Testnums = []
					  for (var testnum in jsonResults["summary"][config_id][system_id][testsuite_id][testname]){
						  Testnums.push(testnum);
					  }
					  Testnums.sort();
					  var benchmarkRun = -1;
					  var compilerOutputs = [];
					  var compilerErrors = [];
					  var runtimeOutputs = [];
					  var runtimeErrors = [];
					  var segmentContent = [];
					  
					  for (var testnum_ind = 0; testnum_ind < Testnums.length; ++testnum_ind){
						  segmentContent.push(getSegmentContents(testsuite_id, testname, Testnums[testnum_ind]));
						  if (jsonResults["summary"][config_id][system_id][testsuite_id][testname][Testnums[testnum_ind]]["result"] == "Pass"){
							  resultsString = resultsString + "P";
							  compilationResultSet.push("For Test Segment " + (testnum_ind + 1) + ": Pass");
							  runtimeResultSet.push("For Test Segment " + (testnum_ind + 1) + ": Pass");
							  benchmarkRun = jsonResults["summary"][config_id][system_id][testsuite_id][testname][Testnums[testnum_ind]]["run_index"];
						  }
						  else if (jsonResults["summary"][config_id][system_id][testsuite_id][testname][Testnums[testnum_ind]]["result"] == "Unknown Section Result"){
							  compilationResultSet.push("For Test Segment " + (testnum_ind + 1) + ": Unknown");
							  runtimeResultSet.push("For Test Segment " + (testnum_ind + 1) + ": Unknown");
							  resultsString = resultsString + "?";
						  }
						  else if (jsonResults["summary"][config_id][system_id][testsuite_id][testname][Testnums[testnum_ind]]["result"] == "Excluded From Run"){
							  compilationResultSet.push("For Test Segment " + (testnum_ind + 1) + ": Unknown");
							  runtimeResultSet.push("For Test Segment " + (testnum_ind + 1) + ": Unknown");
							  resultsString = resultsString + "?";
						  }
						  else if (jsonResults["summary"][config_id][system_id][testsuite_id][testname][Testnums[testnum_ind]]["result"] == "Compilation Failure"){
							  compilationResultSet.push("For Test Segment " + (testnum_ind + 1) + ": Fail");
							  runtimeResultSet.push("For Test Segment " + (testnum_ind + 1) + ": Unknown");
							  resultsString = resultsString + "F";
							  compilerResult = "Fail";
							  if (runtimeResult == "Pass"){
								  runtimeResult = "Unknown";
							  }
							  compilerOutputs.push(jsonResults["runs"][testname][jsonResults["summary"][config_id][system_id][testsuite_id][testname][Testnums[testnum_ind]]["run_index"]]["compilation"]["output"])
							  compilerErrors.push(jsonResults["runs"][testname][jsonResults["summary"][config_id][system_id][testsuite_id][testname][Testnums[testnum_ind]]["run_index"]]["compilation"]["errors"])
						  }
						  else if (jsonResults["summary"][config_id][system_id][testsuite_id][testname][Testnums[testnum_ind]]["result"] == "Runtime Error"){
							  compilationResultSet.push("For Test Segment " + (testnum_ind + 1) + ": Pass");
							  runtimeResultSet.push("For Test Segment " + (testnum_ind + 1) + ": Fail");
							  resultsString = resultsString + "F";
							  runtimeResult = "Fail";
							  runtimeOutputs.push(jsonResults["runs"][testname][jsonResults["summary"][config_id][system_id][testsuite_id][testname][Testnums[testnum_ind]]["run_index"]]["runtime"]["output"]);
							  runtimeErrors.push(jsonResults["runs"][testname][jsonResults["summary"][config_id][system_id][testsuite_id][testname][Testnums[testnum_ind]]["run_index"]]["runtime"]["errors"]);
						  }
						  else if (jsonResults["summary"][config_id][system_id][testsuite_id][testname][Testnums[testnum_ind]]["result"] == "Runtime Failure"){
							  compilationResultSet.push("For Test Segment " + (testnum_ind + 1) + ": Pass");
							  runtimeResultSet.push("For Test Segment " + (testnum_ind + 1) + ": Fail");
							  resultsString = resultsString + "F";
							  runtimeResult = "Fail";
						  }
						  else{
							  resultsString = resultsString + "F";
						  }
					  }
					  
					  $scope.tableContent.push({})
					  $scope.tableContent[counter]["Segment contents"] = segmentContent;
					  $scope.tableContent[counter]["Config name"] = compilerName;
					  $scope.tableContent[counter]["Test system"] = testSystem;
					  $scope.tableContent[counter]["Test name"] = testname;
					  $scope.tableContent[counter]["Compiler result"] = compilerResult;
					  $scope.tableContent[counter]["Runtime result"] = runtimeResult;
					  $scope.tableContent[counter]["Binary path"] = testname;
					  $scope.tableContent[counter]["Test path"] = testname;
					  if (benchmarkRun != -1){
						  $scope.tableContent[counter]["Compiler command"] = jsonResults["runs"][testname][benchmarkRun]["compilation"]["command"];
					  }
					  else{
						  $scope.tableContent[counter]["Compiler command"] = "No Successful Compilation";
					  }
					  $scope.tableContent[counter]["Compiler starting date"] = "";
					  $scope.tableContent[counter]["Compiler ending date"] = "";
					  $scope.tableContent[counter]["Compiler output"] = compilerErrors.join("\n") + "\n" + compilerOutputs.join("\n");
					  $scope.tableContent[counter]["Runtime output"] = runtimeErrors.join("\n") + "\n" + runtimeOutputs.join("\n");
					  $scope.tableContent[counter]["Compilation Result Set"] = compilationResultSet;
					  $scope.tableContent[counter]["Runtime Result Set"] = runtimeResultSet;

					  
					  // Adding test to list of all tests
					  if ($scope.filters.testsOptions.indexOf(testname) == -1) {
						$scope.filters.testsOptions.push(testname);
					  }
					  // Adding compiler name to list of all compilers
					  if ($scope.filters.compilerOptions.indexOf(compilerName) == -1) {
						$scope.filters.compilerOptions.push(compilerName);
					  }
					  // Adding system name to list of all systems
					  if ($scope.filters.systemOptions.indexOf(testSystem) == -1) {
						$scope.filters.systemOptions.push(testSystem);
					  }

					  // Placing all results indexed by testname
					  if ($scope.testsResults[testname] == undefined) {
						$scope.testsResults[testname] = {};
					  }
					  if ($scope.testsResults[testname][testSystem] == undefined) {
						$scope.testsResults[testname][testSystem] = {};
					  }
					  if (compilerResult.indexOf("F") != -1){
						  $scope.testsResults[testname][testSystem][compilerName] = ["Fail CE", counter];
					  }
					  else if (runtimeResult.indexOf("F") != -1){
						  $scope.testsResults[testname][testSystem][compilerName] = ["Fail RE", counter];
					  }
					  else {
						$scope.testsResults[testname][testSystem][compilerName] = ["Pass", counter];
					  }
					  // Placing all compilers by test
					  if ($scope.compilerByTest.get(testname) == undefined) {
						$scope.compilerByTest.set(testname, []);
					  }
					  if ($scope.compilerByTest.get(testname).indexOf(compilerName) == -1) {
						$scope.compilerByTest.get(testname).push(compilerName);
					  }          
					  // Placing all compilers by system
					  if ($scope.compilerBySystems.get(testSystem) == undefined) {
						$scope.compilerBySystems.set(testSystem, []);
					  }
					  if ($scope.compilerBySystems.get(testSystem).indexOf(compilerName) == -1) {
						$scope.compilerBySystems.get(testSystem).push(compilerName);
					  }
					  counter = counter + 1;
					});
				});
			});
		});
		/*
        angular.forEach(jsonResults["runs"], function(value, index) {
		  if (!((index == "systems") || (index == "configs") || index == "Test Info")){
			  for (var testrun = 0; testrun < value.length; testrun++){
				  if (value[testrun]["runtime"]["export"]){
					  //var compilerName = getCompilerName(value[testrun]["compilation"]["config"], index);
					  var config_id = value[testrun]["compilation"]["config"];
					  var system_id = value[testrun]["runtime"]["system"];
					  var testsuite_id = value[testrun]["testsuite_id"];
					  


					  var compilerResult = resultString["compilation"];
					  var runtimeResult = resultString["runtime"];
					  //var compilerResult = getResultString(value[testrun]["compilation"]["result"]);
					  //var runtimeResult = getResultString(value[testrun]["runtime"]["result"]);
					  $scope.tableContent.push({})
					  $scope.tableContent[counter]["Compiler name"] = compilerName;
					  $scope.tableContent[counter]["Test system"] = testSystem;
					  $scope.tableContent[counter]["Test name"] = index;
					  $scope.tableContent[counter]["Compiler result"] = compilerResult;
					  $scope.tableContent[counter]["Runtime result"] = runtimeResult;
					  $scope.tableContent[counter]["Binary path"] = index;
					  $scope.tableContent[counter]["Test path"] = index;
					  $scope.tableContent[counter]["Compiler command"] = value[testrun]["compilation"]["command"];
					  $scope.tableContent[counter]["Compiler starting date"] = value[testrun]["compilation"]["runtime"];
					  $scope.tableContent[counter]["Compiler ending date"] = "";
					  $scope.tableContent[counter]["Compiler output"] = value[testrun]["compilation"]["output"];
					  $scope.tableContent[counter]["Runtime output"] = value[testrun]["runtime"]["output"];
					  // Adding test to list of all tests
					  if ($scope.filters.testsOptions.indexOf(testName) == -1) {
						$scope.filters.testsOptions.push(testName);
					  }
					  // Adding compiler name to list of all compilers
					  if ($scope.filters.compilerOptions.indexOf(compilerName) == -1) {
						$scope.filters.compilerOptions.push(compilerName);
					  }
					  // Adding system name to list of all systems
					  if ($scope.filters.systemOptions.indexOf(testSystem) == -1) {
						$scope.filters.systemOptions.push(testSystem);
					  }

					  // Placing all results indexed by testname
					  if ($scope.testsResults[testName] == undefined) {
						$scope.testsResults[testName] = {};
					  }
					  if ($scope.testsResults[testName][testSystem] == undefined) {
						$scope.testsResults[testName][testSystem] = {};
					  }
					  if (compilerResult.indexOf("F") != -1){
						  $scope.testsResults[testName][testSystem][compilerName] = ["FAIL CE", counter];
					  }
					  else if (runtimeResult.indexOf("F") != -1){
						  $scope.testsResults[testName][testSystem][compilerName] = ["FAIL RE", counter];
					  }
					  else {
						$scope.testsResults[testName][testSystem][compilerName] = ["PASS", counter];
					  }
					  counter = counter + 1;
					  // Placing all compilers by test
					  if ($scope.compilerByTest.get(testName) == undefined) {
						$scope.compilerByTest.set(testName, []);
					  }
					  if ($scope.compilerByTest.get(testName).indexOf(compilerName) == -1) {
						$scope.compilerByTest.get(testName).push(compilerName);
					  }          
					  // Placing all compilers by system
					  if ($scope.compilerBySystems.get(testSystem) == undefined) {
						$scope.compilerBySystems.set(testSystem, []);
					  }
					  if ($scope.compilerBySystems.get(testSystem).indexOf(compilerName) == -1) {
						$scope.compilerBySystems.get(testSystem).push(compilerName);
					  }
				  }
			  }
			}
        });
		*/
        console.log("Finished loading results");
        $scope.loadingResults = false;
      }

      $scope.testsResults = {};
      $scope.compilerBySystems = new Map();
      $scope.compilerByTest = new Map();

      // For the stats display
      $scope.getListOfSystems = function() {
        var returnList = [];
        $scope.compilerBySystems.forEach( function(value, key) {
            returnList.push(key);
        });
        return $filter('filterSystems')(returnList, $scope.filters);
      };
      $scope.getListOfCompilersSystems = function() {
        var returnList = [];
        $scope.compilerBySystems.forEach( function(compiler, system) {
          if ($filter('filterSystems')([system], $scope.filters).length != 0) {
            returnList.push.apply(returnList, compiler); 
          }
        });
        return $filter('filterCompilers')(returnList, $scope.filters);
      };

      $scope.getResultsSystemCompiler = function (test) {
        var returnList = [];
        $scope.compilerBySystems.forEach( function(systemCompilers, systemName) {
          if ($filter('filterSystems')([systemName], $scope.filters).length != 0) {
            var filteredSystemCompilers = $filter('filterCompilers')(systemCompilers, $scope.filters);
            filteredSystemCompilers.forEach(function (compiler) {
              var result=$scope.testsResults[test];
              result = result != undefined ? result[systemName]:undefined;
              result = result != undefined ? result[compiler]:undefined;
            
              returnList.push(result);
            });
          }
        });
        return returnList;
      }

      $scope.getListOfTests = function() {
        return $filter('filterTests')($scope.filters.testsOptions, $scope.filters);
      }

      // To sort results by column
      $scope.order = {};
      $scope.order.reverseSort = false;
      $scope.order.orderByField = 'Test path';
      $scope.colsToDisplay = ['Test name', 'Test system', 'Config name', 'Compiler result', 'Runtime result'];

      // To filter results by column
      $scope.filters = {};
      $scope.filters.testsOptions = [];
      $scope.filters.searchFilter = "";
      $scope.filters.compilerOptions = [];
      $scope.filters.compilerFilter = [];
      $scope.filters.systemOptions = [];
      $scope.filters.systemFilter = [];
      $scope.filters.compilerResultFilter = "Both";
      $scope.filters.runResultFilter = "Both";
      

      // Function for ordering table See here: 
      // https://stackoverflow.com/questions/23134773/angularjs-ngrepeat-orderby-when-property-key-has-spaces
      $scope.predicate = function(val) {
        return val[$scope.order.orderByField];
      }

      // function to verify if PASS is preset
      $scope.testPassed = function(result) {
        if (typeof result === 'string') {
          return result.indexOf("Pass") !== -1;
        } 
        return false;      }
      $scope.testFailed = function(result) {
        if (typeof result === 'string') {
          return result.indexOf("Fail") !== -1;
        } 
        return false;
      }
	  $scope.testUnknown = function(result) {
		  if (typeof result === 'string'){
			  return result.indexOf("Unknown") !== -1;
		  }
		  return false;
	  }

      // More information results modal
      $scope.openResultModal = function (value) {
        var modalInstance = $uibModal.open({
          animation: true,
          component: 'modalResults',
          size: 'lg',
          resolve: {
            resultEntry: function() {
              return value;
            },
            ansi2html: function() {
              return ansi2html;
            }

          }
        });

        modalInstance.result.then(function () {
          $log.info('Modal closed');
        }, function () {
          $log.info('Modal dismissed');
        });
      }
      $scope.loadResults();

  }]);

  var modalResultsCtrl = 

  accvv.component('modalResults', {
    template: `
    <div class="modal-content">
    <div class="modal-header">
        <h3>
            <span class="label label-info" id="qid">{{$ctrl.resultEntry['Test name']}}</span>
        </h3>
        <h4>
            <span class="label label-primary">Path:</span><span>  {{$ctrl.resultEntry['Test path']}}</span> <br/>
        </h4>
        <h4>
            <span class="label label-primary">Compiler:</span><span>  {{$ctrl.resultEntry['Compiler name']}}</span>
        </h4>
    </div>
    <div class="modal-body">
		<div class="outputsContainer">
			<table width=95%>
				<tr ng-repeat="entry in $ctrl.resultEntry['Compilation Result Set']">
					<td>
						<div style="margin-bottom: 15px" >
							<label class="btn btn-lg btn-block" ng-click="$ctrl.segmentActive[$index]=!$ctrl.segmentActive[$index]" ng-class="{'btn-success': $ctrl.testPassed($ctrl.resultEntry['Runtime Result Set'][$index]) && $ctrl.testPassed($ctrl.resultEntry['Compilation Result Set'][$index]), 'btn-danger': !$ctrl.testPassed($ctrl.resultEntry['Runtime Result Set'][$index]) || !$ctrl.testPassed($ctrl.resultEntry['Compilation Result Set'][$index])}">
								Segment {{$index+1}}
							</label>
							<div class="slideDown" ng-show="$ctrl.segmentActive[$index]">
								<div style="margin-bottom: 15px" >
									<label class="btn btn-lg btn-block" 
									ng-click="$ctrl.compilerOutputActive[$index]=!$ctrl.compilerOutputActive[$index]"
									ng-class="{'btn-success': $ctrl.testPassed($ctrl.resultEntry['Compilation Result Set'][$index]), 'btn-danger': $ctrl.testFailed($ctrl.resultEntry['Compilation Result Set'][$index])}">
									<i class="glyphicon glyphicon-chevron-right"></i>Compiler Result {{$ctrl.resultEntry['Compilation Result Set'][$index]}}
									</label>
									<div class="slideDown" ng-show="$ctrl.compilerOutputActive[$index]">
										<div class="well well-sm">
												<strong>Compiler command: </strong>{{$ctrl.resultEntry['Compiler command']}}
										</div>
										<div class="well well-sm">
												<strong>Compilation time range: </strong>{{$ctrl.resultEntry['Compiler starting date']}} - {{$ctrl.resultEntry['Compiler ending date']}}
										</div>
										<div class="well well-sm resultOutput">
												<strong>Compilation output: </strong><br/><span class="outputText" ng-bind-html="$ctrl.color2html($ctrl.resultEntry['Compiler output'])"></span>
										</div>
									</div>
								</div>
								<div style="margin-bottom: 15px" >
									<label class="btn btn-lg btn-primary btn-block" 
									ng-click="$ctrl.runtimeOutputActive[$index]=!$ctrl.runtimeOutputActive[$index]"
									ng-class="{'btn-success': $ctrl.testPassed($ctrl.resultEntry['Runtime Result Set'][$index]), 'btn-danger': $ctrl.testFailed($ctrl.resultEntry['Runtime Result Set'][$index])}">
											<i class="glyphicon glyphicon-chevron-right"></i>Runtime Result {{$ctrl.resultEntry['Runtime Result Set'][$index]}}
									</label>
									<div class="slideDown" ng-show="$ctrl.runtimeOutputActive[$index]">
										<div class="well well-sm">
												<strong>Run binary path: </strong>{{$ctrl.resultEntry['Binary path']}}
										</div>
										<div class="well well-sm resultOutput">
												<strong>Run output: </strong><br/><span class="outputText" ng-bind-html="$ctrl.color2html($ctrl.resultEntry['Runtime output'])"></span>
										</div>
									</div>
								</div>
								<div style="margin-bottom: 15px">
									<label class="btn btn-lg btn-primary btn-block"
									ng-click="$ctrl.segmentContentActive[$index]=!$ctrl.segmentContentActive[$index]"
									ng-class="{'btn-success': $ctrl.testPassed($ctrl.resultEntry['Runtime Result Set'][$index]) && $ctrl.testPassed($ctrl.resultEntry['Compilation Result Set'][$index]), 'btn-danger': !$ctrl.testPassed($ctrl.resultEntry['Runtime Result Set'][$index]) || !$ctrl.testPassed($ctrl.resultEntry['Compilation Result Set'][$index])}">
										Segment Contents:
									</label>
									<div style="white-space: pre" class="well well-lg resultOutput" ng-show="$ctrl.segmentContentActive[$index]">
										<strong>Segment Contents: <br/></strong>{{$ctrl.resultEntry['Segment contents'][$index]}}
									</div>
								</div>
							</div>
						</div>
					</td>
				</tr>
			</table>
		</div>
    </div>
    <div class="modal-footer text-muted">
        <span id="answer"></span>
    `,
    controllerAs: '$ctrl',
    bindings: {
      resolve: '<',
      close: '&'
    },
    controller: function($sce) {
      var $ctrl = this;
	  $ctrl.compilerOutputActive = false;
	  $ctrl.runtimeOutputActive = false;
	  $ctrl.segmentContentActive = false;
	  $ctrl.segmentActive = false
      $ctrl.$onInit = function () {
        $ctrl.resultEntry = $ctrl.resolve.resultEntry;
        $ctrl.ansi2html = $ctrl.resolve.ansi2html
		$ctrl.compilerOutputActive = new Array($ctrl.resultEntry["Compilation Result Set"].length).map(function (x, i) { return false });
	    $ctrl.runtimeOutputActive = new Array($ctrl.resultEntry["Runtime Result Set"].length).map(function (x, i) { return false});
		$ctrl.segmentContentActive = new Array($ctrl.resultEntry["Runtime Result Set"].length).map(function (x, i) { return false});
		$ctrl.segmentActive = new Array($ctrl.resultEntry["Runtime Result Set"].length).map(function (x, i) { return false});
      }
      $ctrl.ok = function () {
        $uibModalInstance.close();
      };
    
      $ctrl.cancel = function () {
        $uibModalInstance.dismiss('cancel');
      };
  
      // function to verify if PASS is preset
      $ctrl.testPassed = function(result) {
        if (typeof result === 'string') {
          return result.indexOf("Pass") !== -1;
        } 
        return false;      }
	  $ctrl.testUnknown = function(result) {
		  if (typeof result === 'string') {
			return result.indexOf("Unknown") !== -1;
		  }
		  return false;
	  }
	  
      $ctrl.testFailed = function(result) {
        if (typeof result === 'string') {
          return result.indexOf("Pass") == -1 && result.indexOf("Unknown") == -1;
        } 
        return false;
      }
      $ctrl.color2html = function(outputText) {
        return $sce.trustAsHtml($ctrl.ansi2html.toHtml(outputText).replace(/\n/g, '<br/>'));
      }
  
    }
  });

  accvv.filter('filterCompilers', function(){
    return function(items, filters) {
      var filtered = [];
      angular.forEach(items, function(item){
        var containsCompiler = false;
        angular.forEach(filters.compilerFilter, function(compiler) {
          if (compiler == "" || item == compiler)
            containsCompiler = true;
        });
        if (containsCompiler || filters.compilerFilter == "") {
          filtered.push(item);
        }
      });
      return filtered;
    }
  })
  accvv.filter('filterTests', function(){
    return function(items, filters) {
      var filtered = [];
      angular.forEach(items, function(item){
        if (filters.searchFilter == "" || item.includes(filters.searchFilter)) {
          filtered.push(item);
        }
      });
      return filtered;
    }
  })
  accvv.filter('filterSystems', function(){
    return function(items, filters) {
      var filtered = [];
      angular.forEach(items, function(item){
        var containsSystem = false;
        angular.forEach(filters.systemFilter, function(system) {
          if (system == "" || item == system)
          containsSystem = true;
        });
        if (containsSystem || filters.systemFilter == "") {
          filtered.push(item);
        }
      });
      return filtered;
    }
  })
  
  accvv.filter('applyResultsFilter', function() {
      return function (items, filters) {
        var filtered = [];
        if (filters.searchFilter == "" && filters.compilerFilter == "" && filters.systemFilter == "" && filters.compilerResultFilter == "Both" && filters.runResultFilter == "Both") {
          return items;
        }
		var date = new Date();
		console.log(date.getTime());
        angular.forEach(items, function(item) {
          var removeItem = false;
          // filter by test name 
          if (filters.searchFilter != "" && !item['Test path'].includes(filters.searchFilter)){
            removeItem = true;
		  }
          // Filter by compiler name 
          var containsCompiler = false;
          angular.forEach(filters.compilerFilter, function(compiler) {
            if (compiler != "" && item['Config name'] == compiler)
              containsCompiler = true;
          });
          if (filters.compilerFilter != "" && !containsCompiler){
            removeItem = true;
		  }
          // Filter by system name
          var containsSystem = false;
          angular.forEach(filters.systemFilter, function(system) {
            if (system != "" && item['Test system'] == system)
              containsSystem = true;
          });
          if (filters.systemFilter != "" && !containsSystem){
            removeItem = true;
		  }
          // Filter by compiler result
          if (filters.compilerResultFilter != "Both" && !item['Compiler result'].includes(filters.compilerResultFilter)){
            removeItem = true;
		  }
          // Filter by Runtime result
          if (filters.runResultFilter != "Both" && !item['Runtime result'].includes(filters.runResultFilter)){
            removeItem = true;
		  }
		  
          if (!removeItem) 
            filtered.push(item);
        });
		date = new Date();
		console.log(date.getTime());
        return filtered;
      };
  });
  
})(window.angular);
