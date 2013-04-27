#ifndef CONSTANTS_H
#define CONSTANTS_H

enum graphConstants
{
  constGraphS11DB = 1,
  constGraphS11Ang = 2,
  constRho = 3,
  constImpedMag = 4,
  constImpedAng = 5,
  constSerR = 6,
  constSerReact  = 7,
  constParR = 8,
  constParReact = 9,
  constSerC = 10,
  constSerL = 11,
  constParC = 12,
  constParL = 13,
  constSWR = 14,
  constIntermedS11DB = 15,
  constIntermedS11Ang = 16,
  constMagDBM = 17,
  constMagWatts = 18,
  constMagV = 19,
  constMagDB = 20,
  constMagRatio = 21,
  constAngle = 22,
  constTheta = 23,
  constGD = 24,
  constReturnLoss = 25,
  constInsertionLoss = 26,
  constReflectPower = 27,
  constComponentQ = 28,
  constAdmitMag = 29,
  constAdmitAng = 30,
  constConductance = 31,
  constSusceptance = 32,
  constRawAngle = 33,
  constAux0 = 34,
  constAux1 = 35,
  constAux2 = 36,
  constAux3 = 37,
  constAux4 = 38,
  constAux5 = 39,
  constNoGraph = 40
};

enum hardwareConstants
{
  constHardware = 0,
  constGrid = 1,
  constTrace = 2,
  constSweep = 3,
  constMarker = 4,
  constBand = 5,
  constBase = 6,
  constGraphData = 7,
  constModeData = 8

};

enum lptConstants
{
  contclear = 11,
  STRB = 10,
  AUTO = 9,
  INIT = 15,
  SELT = 3,
  INITSELT =  7,
  STRBAUTO = 8
};


enum twoPortConstants
{
  //The following constants are for two-port graphs. They do not have values
  //distinct from those for regular graphs ver116-1b
  constTwoPortS11DB=1,
  constTwoPortS21DB=3,
  constTwoPortS12DB=5,
  constTwoPortS22DB=7,
  constTwoPortS11Ang=2,
  constTwoPortS21Ang=4,
  constTwoPortS12Ang=6,
  constTwoPortS22Ang=8,

  //The following can be used directly (after subtracting constTwoPortMatchedS11DB-1) to index TwoPortMatchedSParam to retrieve the data
  //Note that the constant for an angle is always one more than that for its corresponding DB
  constTwoPortMatchedS11DB=9,
  constTwoPortMatchedS11Ang=10,
  constTwoPortMatchedS21DB=11,
  constTwoPortMatchedS21Ang=12,
  constTwoPortMatchedS12DB =13,
  constTwoPortMatchedS12Ang =14,
  constTwoPortMatchedS22DB=15,
  constTwoPortMatchedS22Ang=16,

  //The following do not directly index an array,
  constTwoPortKStability=17,
  constTwoPortMuStability=18
};


#endif // CONSTANTS_H
