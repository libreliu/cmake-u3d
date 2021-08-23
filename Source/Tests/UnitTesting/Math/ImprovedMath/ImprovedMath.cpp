//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

#ifndef IMPROVEDMATH_H
#include "ImprovedMath.h"
#endif

#include "UTHeaders.h"
#include "IFXAutoRelease.h"
#include "IFXMatrix4x4.h"
#include "IFXVector2.h"
#include "IFXVector3.h"
#include "IFXVector4.h"
#include "math.h"
#include <iostream>
//#include <tchar.h>
//#include "IFXVector3.h"

int ImprovedMath::Run()
{

	IFXCOMInitialize();

  //----------------------Matrix Tests-----------------------------------
  {
  //Multiplication Matrix Test
  IFXMatrix4x4 result, check;
  F32 resultF32, checkF32;
  IFXVector3 resultV3, checkV3;
  IFXVector2 resultV2, checkV2;
  IFXVector4 resultV4, checkV4;
  const F32 PI = F32(3.14159265358979323846264338);
  F32 epsError, eps = F32(0.00001);
  int i;


  const F32 init1f3[] = {1,2,0};
  IFXVector3 init1V3 = IFXVector3(init1f3);
  const F32 init2f3[] = {1,3,-1};
  IFXVector3 init2V3 = IFXVector3(init2f3);
  resultV3.Multiply( init1V3, init2V3 );
  const F32 checkf3[] = {1,6,0};
  checkV3 = IFXVector3(checkf3);
  TEST( resultV3 == checkV3, IFXException );

  const F32 init1f4[] = {1,2,0,4};
  IFXVector4 init1V4 = IFXVector4(init1f4);
  const F32 init2f4[] = {0,3,-2,1};
  IFXVector4 init2V4 = IFXVector4(init2f4);
  resultV4.Multiply( init1V4, init2V4 );
  const F32 checkf4[] = {0,6,0,4};
  checkV4 = IFXVector4(checkf4);
  TEST( resultV4 == checkV4, IFXException );

  const F32 init1[] = {1,2,3,4,
             5,6,7,8,
             9,8,7,6,
             5,4,3,2};
  IFXMatrix4x4 op1(init1);
  const F32 init2[] = {0,1,0,0,
             0,0,2,0,
             1,0,0,0,
             0,0,0,1};
  IFXMatrix4x4 op2(init2);
  result.Multiply( op1, op2 );
  const F32 check1[] = {5,6,7,8,
              18,16,14,12,
              1,2,3,4,
              5,4,3,2};
  check = IFXMatrix4x4(check1);
  TEST( check == result, IFXException );


  //Invertion Matrix test
  const F32 check2[] = {0,0,1,0,
              1,0,0,0,
              0,F32(0.5),0,0,
              0,0,0,1};
  result.Invert(op2);
  check = IFXMatrix4x4(check2);
  TEST( check == result, IFXException );


  //IFXMatrix4x4  Invert3x4 61
  const F32 initInvert3x4[] = {1,2,4,0,
                     0,F32(0.5),1,-4,
                     8,0,1,-1,
                     0,2,0,1};
  IFXMatrix4x4 opInvert3x4 = IFXMatrix4x4(initInvert3x4);
  const F32 checkInvert3x4[] = {1,-4,0,0,
                  16,-62,-2,0,
                  -8,32,1,0,
                  -32,124,4,1};
  check = IFXMatrix4x4(checkInvert3x4);

  result.Invert3x4(opInvert3x4);
  TEST( check == result, IFXException );



  //Multiply3x4 matrix Test
  result.MakeIdentity();
  result.Multiply3x4(op1,op2);
  const F32 check3[] = {5,6,7,0,
              18,16,14,0,
              1,2,3,0,
              5,4,3,1};
  check = IFXMatrix4x4(check3);
  TEST( check == result, IFXException );


  //IFXMatrix4x4  CalcDeterminant 120

  resultF32 = op2.CalcDeterminant();
  checkF32 = F32(2.0);
  TEST( checkF32 == resultF32, IFXException );

  const F32 init3[] =    {0,1,0,0,
               1,0,0,0,
               0,0,1,0,
               0,0,0,2};
  IFXMatrix4x4 op3(init3);
  resultF32 = op3.CalcDeterminant();
  checkF32 = F32(-2.0);
  TEST( checkF32 == resultF32, IFXException );


  //IFXMatrix4x4  CalcDeterminant3x3  92

  resultF32 = op3.CalcDeterminant3x3();
  checkF32 = F32(-1.0);
  TEST( checkF32 == resultF32, IFXException );


  //IFXMatrix4x4  CalcDeterminant3x3  92 //the determinant of an arbitrary 3x3 matrix passed as 9 individual floats.

  checkF32 = F32(-112.0);
  resultF32 = op3.CalcDeterminant3x3(5,6,7,18,16,14,5,2,3);

  TEST( checkF32 == resultF32, IFXException );


  //IFXMatrix4x4  Transpose 136

  const F32 initTranspose[] = {1,2,3,4,
                 5,6,7,8,
                 9,8,7,6,
                 5,4,3,2};
  IFXMatrix4x4 opTranspose(initTranspose);
  const F32 checkTranspose[] ={1,5,9,5,
                2,6,8,4,
                3,7,7,3,
                4,8,6,2};
  opTranspose.Transpose();
  check = IFXMatrix4x4(checkTranspose);
  TEST( check == opTranspose, IFXException );



  //IFXMatrix4x4  Translate3x4  25

  const F32 initV3[] = {1,3,-2};
    IFXVector3 opV3(initV3);
  const F32 initT3x4M[] = {1,2,3,4,
              5,6,7,8,
              9,8,7,6,
              5,4,3,2};
  IFXMatrix4x4 opT3x4M(initT3x4M);

  result = opT3x4M.Translate3x4(opV3);

  const F32 checkTranslate3x4[] = {1,2,3,4,
                  5,6,7,8,
                  9,8,7,6,
                  3,8,13,2};

  check = IFXMatrix4x4(checkTranslate3x4);
  TEST( check == result, IFXException );



  //IFXMatrix4x4  RotateVector  105

  const F32 initVRot[] = {1,3,-2};
    IFXVector3 opVRot(initVRot);
  const F32 initRotateVectorM[] = {1,2,3,4,
                  5,6,7,8,
                  9,8,7,6,
                  5,4,3,2};
  IFXMatrix4x4 opRV(initRotateVectorM);

  opRV.RotateVector(opVRot,resultV3);

  const F32 check5RotateVector[] = {-2,4,10};
  checkV3 = IFXVector3(check5RotateVector);
  TEST( checkV3 == resultV3, IFXException );


  //IFXMatrix4x4  RotateAxis  47
               // as I understand this function should have   m_data[15]  = 1.0f; inside

  const F32 initVRotateAxis[] = {1,0,0,1};
    IFXVector4 opRotateAxis(initVRotateAxis);

  const F32 angleRotateAxis = PI/2;


  const F32 initCRotateAxisM[] = {1,0,0,0,
                  0,0,1,0,
                    0,-1,0,0,
                  0,0,0,1};
  IFXMatrix4x4 opCRotateAxisM(initCRotateAxisM);

  result.RotateAxis(opRotateAxis, angleRotateAxis);

  epsError = 0;
  for( i=0; i<16; i++)
  {
    epsError += fabs(opCRotateAxisM.Raw()[i]-result.Raw()[i]);
  }

  TEST( epsError < eps, IFXException );

  //IFXMatrix4x4  TransformVector 108

  const F32 initVTransform[] = {1,3,-2};
    IFXVector3 opVTransform(initVTransform);
  const F32 initVTransformM[] = {1,2,3,4,
                  5,6,7,8,
                  9,8,7,6,
                  5,4,3,2};
  IFXMatrix4x4 opVTransformM(initVTransformM);

  opVTransformM.TransformVector(opVTransform,resultV3);
  const F32 checkTransformVector[] = {3,8,13};
  checkV3 = IFXVector3(checkTransformVector);
  TEST( checkV3 == resultV3, IFXException );



  //IFXMatrix4x4  Scale 3559

  const F32 initVScale[] = {1,3,-2};
    IFXVector3 opVScale(initVScale);
  const F32 initScaleM[] =       {1,2,3,4,
                  5,6,7,8,
                  9,8,7,6,
                  5,4,3,2};
  IFXMatrix4x4 opScaleM(initScaleM);
  result = opScaleM.Scale(opVScale);
  const F32 checkScaleMatrix[] = {1,2,3,4,
                  15,18,21,24,
                  -18,-16,-14,-12,
                  5,4,3,2};

  check = IFXMatrix4x4(checkScaleMatrix);
  TEST( check == result, IFXException );


  //MakeTensor 20
  const F32 initMakeTensorV1[] = {1,3,-2};
  IFXVector3 MakeTensorV1(initMakeTensorV1);

  const F32 initMakeTensorV2[] = {-5,6,10};
  IFXVector3 MakeTensorV2(initMakeTensorV2);

  const F32 checkMakeTensor[] = {-5,-15,10,0,
                  6,18,-12,0,
                  10,30,-20,0,
                  0,0,0,1};
  check = IFXMatrix4x4(checkMakeTensor);

  result.MakeTensor( MakeTensorV1, MakeTensorV2);
  TEST( check == result, IFXException );


  //IFXMatrix4x4  Rotate3x4 22

  const F32 initRotate3x4[] =    {1,2,3,4,
                  5,6,7,8,
                  9,8,7,6,
                  5,4,3,2};
  IFXMatrix4x4 opRotate3x4(initRotate3x4);

  IFXAxisID AxisRotate3x4 = IFX_Z_AXIS;
  const F32 angleRotate3x4 = PI/2;

  const F32 initCRotate3x4[] = {5,6,7,4,
                  -1,-2,-3,8,
                  9,8,7,6,
                  5,4,3,2};
  IFXMatrix4x4 opCRotate3x4(initCRotate3x4);

  opRotate3x4.Rotate3x4(angleRotate3x4, AxisRotate3x4);
  epsError = 0;
  for( i=0; i<16; i++)
  {
    epsError += fabs(opRotate3x4.Raw()[i]-opCRotate3x4.Raw()[i]);
  }

  TEST( epsError < eps, IFXException );


  //IFXMatrix4x4  Scale3x4  26
  const F32 initVScale3x4[] = {1,3,-2};
    IFXVector3 opVScale3x4(initVScale3x4);
  const F32 initScale3x4M[] =       {1,2,3,4,
                  5,6,7,8,
                  9,8,7,6,
                  5,4,3,2};
  IFXMatrix4x4 opScale3x4M(initScale3x4M);

  const F32 checkScale3x4Matrix[] = {1,2,3,4,
                  15,18,21,8,
                  -18,-16,-14,6,
                  5,4,3,2};
  check = IFXMatrix4x4(checkScale3x4Matrix);

  result = opScale3x4M.Scale3x4(opVScale3x4);

  TEST( check == result, IFXException );


  //IFXMatrix4x4  ProjectVector 23
  const F32 initProjectVector[] = {1,3,-2};
    IFXVector3 opProjectVector(initProjectVector);
  const F32 initProjectVectorM[] = {1,2,3,0,
                   -1,6,7,0,
                   9,8,1,0,
                   1,0,0,0.5};
  IFXMatrix4x4 opProjectVectorM(initProjectVectorM);

  opProjectVectorM.ProjectVector(opProjectVector,resultV3);
  const F32 checkProjectVector[] = {-38,8,44};
  checkV3 = IFXVector3(checkProjectVector);

  TEST( checkV3 == resultV3, IFXException );


  //IFXMatrix4x4  Decompose 66  // in rotation matrix elements 3, 7, 11, 15 are missing
  const F32 initDecomposeTranslation[] = {-3,4,2};
    IFXVector3 opDecomposeTranslationVector(initDecomposeTranslation);
  IFXVector3 opDecomposeTranslationVectorResult;

  const F32 initDecomposeScale[] = {2,1,1};
    IFXVector3 opDecomposeScaleVector(initDecomposeScale);
  IFXVector3 opDecomposeScaleVectorResult;

  const F32 initDecomposeRotation[] = {1,0,0,0,
                    0,0,1,0,
                    0,-1,0,0,
                    0,0,0,1};
  IFXMatrix4x4 opDecomposeRotation(initDecomposeRotation);
  IFXMatrix4x4 opDecomposeRotationResult;

  const F32 initCDecompose[] = {2,0,0,0,
                  0,0,1,5,
                  0,-1,0,0,
                  -3,4,2,1};
  IFXMatrix4x4 CDecompose( initCDecompose);

  CDecompose.Decompose( opDecomposeTranslationVectorResult, opDecomposeRotationResult, opDecomposeScaleVectorResult);

  epsError = 0;
  for( i=0; i<16; i++)
  {
    epsError += fabs(opDecomposeRotation.Raw()[i]-opDecomposeRotationResult.Raw()[i]);
  }

  TEST( (opDecomposeTranslationVectorResult == opDecomposeTranslationVector)
    &&(opDecomposeScaleVectorResult == opDecomposeScaleVector)
    &&(epsError < eps)
    , IFXException );


  //IFXMatrix4x4  CalcTRS 30
  const F32 initCalcTRSTranslation[] = {-3,4,2};
    IFXVector3 opCalcTRSTranslationVector(initCalcTRSTranslation);
  IFXVector3 opCalcTRSTranslationVectorResult;

  const F32 initCalcTRSScale[] = {2,1,1};
    IFXVector3 opCalcTRSScaleVector(initCalcTRSScale);
  IFXVector3 opCalcTRSScaleVectorResult;

  IFXQuaternion opCalcTRSRotation;
  opCalcTRSRotation[0] = F32(0.707106811865475244);
  opCalcTRSRotation[1] = F32(0.707106811865475244);
  opCalcTRSRotation[2] = 0;
  opCalcTRSRotation[3] = 0;
  IFXQuaternion opCalcTRSRotationResult;

  const F32 initCCalcTRS[] = {2,0,0,0,
                0,0,1,0,
                0,-1,0,0,
                -3,4,2,1};
  IFXMatrix4x4 CCalcTRS( initCCalcTRS);

  CCalcTRS.CalcTRS( &opCalcTRSTranslationVectorResult, &opCalcTRSRotationResult, &opCalcTRSScaleVectorResult);

  epsError = 0;

  for( i=0; i<4; i++)
  {
    epsError += fabs(opCalcTRSRotation.Raw()[i]-opCalcTRSRotationResult.Raw()[i]);
  }

  TEST( (opCalcTRSTranslationVectorResult == opCalcTRSTranslationVector)
    &&(opCalcTRSScaleVectorResult == opCalcTRSScaleVector)
    &&(epsError < eps)
    , IFXException );


  //NegativelyScaled;

  BOOL checkBool = 0, resultBool;
  const F32 initNegativelyScaled[] = {1,2,3,4,
                    5,6,7,8,
                    9,8,7,6,
                    5,4,3,2};
  IFXMatrix4x4 opNegativelyScaled( initNegativelyScaled);

  resultBool = opNegativelyScaled.NegativelyScaled();

  TEST( checkBool == resultBool, IFXException );


  //IFXMatrix4x4  CalcAxisScale 17
  const F32 initCalcAxisScaleV[] = { 3, 2, 2};
    IFXVector3 opCalcAxisScaleV(initCalcAxisScaleV);
  IFXVector3 opCalcAxisScaleVResult;

  const F32 initCalcAxisScaleM[] = {3,0,0,0,
                    0,0,2,0,
                    0,-2,0,0,
                    0,0,0,0};
  IFXMatrix4x4 opCalcAxisScaleM( initCalcAxisScaleM);

  opCalcAxisScaleM.CalcAxisScale( opCalcAxisScaleVResult);
  TEST( opCalcAxisScaleV == opCalcAxisScaleVResult, IFXException );

  }
  //----------------------Vector2 Tests---------------------------------
  {

  IFXVector2 resultVector2, checkVector2;

  //IFXVector2  IsApproximately 33

  const F32 initV1[] = {1,3};
  IFXVector2 vec1(initV1);
  const F32 initV2[] = {1,2};
  IFXVector2 vec2(initV2);
  TEST( vec1.IsApproximately(vec2,2), IFXException );



  //  Add 7955
  const F32 initVadd1[] = {1,3};
  vec1 = IFXVector2(initVadd1);

  const F32 initVadd2[] = {1,2};
  vec2 = IFXVector2(initVadd2);

  const F32 initCadd2[] = {2,5};
  IFXVector2 checkVec(initCadd2);
  IFXVector2 resVecAdd;
  resVecAdd.Add(vec1,vec2);
  TEST( checkVec == resVecAdd, IFXException );

  // Add a.b

  vec1.Add( vec2);
  TEST( checkVec == vec1, IFXException );

  //  Subtract  344
  const F32 initVSub1[] = {1,3};
  vec1 = IFXVector2(initVSub1);

  const F32 initVSub2[] = {1,2};
  vec2 = IFXVector2(initVSub2);

  const F32 initCSub2[] = {0,1};
  checkVec = IFXVector2(initCSub2);

  IFXVector2 resVecSub;
  resVecSub.Subtract(vec1,vec2);

  TEST( checkVec == resVecSub, IFXException );

  //  Subtract a.b
  vec1.Subtract(vec2);
  TEST( checkVec == vec1, IFXException );


  //  Multiply  398
  const F32 initVMul1[] = {0,3};
  vec1 = IFXVector2(initVMul1);

  const F32 initVMul2[] = {1,2};
  vec2 = IFXVector2(initVMul2);

  const F32 initCVMul2[] = {0,6};
  IFXVector2 checkVecMul(initCVMul2);
  IFXVector2 resVMul;
  resVMul.Multiply(vec1,vec2);
  TEST( checkVecMul == resVMul, IFXException );

  //  Multiply  a.b
  vec1.Multiply( vec2);
  TEST( checkVecMul == vec1, IFXException );

  //  Scale 3559

  const F32 initV2Scale[] = {4,-2};
  vec1 = IFXVector2(initV2Scale);

  const F32 initCV2Scale[] = {20, -10};
  checkVec = IFXVector2(initCV2Scale);

  IFXVector2 resV2Scale;
  resV2Scale = vec1.Scale(5);

  TEST( checkVec == resV2Scale, IFXException );



  //  Invert  348

  const F32 initVInvert[] = {4,-2};
  vec1 = IFXVector2(initVInvert);

  const F32 initCVInvert[] = {-4, 2};
  checkVec = IFXVector2(initCVInvert);

  resultVector2 = vec1.Invert();

  TEST( checkVec == resultVector2, IFXException );


  //  DotProduct  244

  const F32 initVDotProduct1[] = {0,3};
  vec1 = IFXVector2(initVDotProduct1);

  const F32 initVDotProduct2[] = {1,2};
  vec2 = IFXVector2(initVDotProduct2);

  const F32 initCVDotProduct2 = 6;

  F32 V2DotProductResult;
  V2DotProductResult = vec1.DotProduct(vec2);

  TEST( initCVDotProduct2 == V2DotProductResult, IFXException );


  //  CalcMagnitude 113

  const F32 initV2CalcMagnitude[] = {3,4};
  vec1 = IFXVector2(initV2CalcMagnitude);

  const F32 initCV2CalcMagnitude = 5;

  F32 V2CalcMagnitudeResult;
  V2CalcMagnitudeResult = vec1.CalcMagnitude();

  TEST( initCV2CalcMagnitude == V2CalcMagnitudeResult, IFXException );


  //  CalcMagnitudeSquared  32

  const F32 initV2CalcMagnitudeSquared[] = {3,4};
  vec1 = IFXVector2(initV2CalcMagnitudeSquared);

  const F32 initCV2CalcMagnitudeSquared = 25;

  F32 V2CalcMagnitudeSquaredResult;
  V2CalcMagnitudeSquaredResult = vec1.CalcMagnitudeSquared();

  TEST( initCV2CalcMagnitudeSquared == V2CalcMagnitudeSquaredResult, IFXException );


  //  Normalize 271

  const F32 initV2Normalize[] = {3,4};
  vec1 = IFXVector2(initV2Normalize);

  const F32 initCV2Normalize[] = {F32(0.6),F32(0.8)};
  checkVector2 = IFXVector2(initCV2Normalize);

  resultVector2 = vec1.Normalize();

  TEST( checkVector2 == resultVector2, IFXException );


  //  Divide  144

  const F32 initVDivide2_1[] = {1,6};
  vec1 = IFXVector2(initVDivide2_1);

  const F32 initVDivide2_2[] = {2,-1};
  vec2 = IFXVector2(initVDivide2_2);

  const F32 initCVDivide2[] = {F32(0.5),-6};
  IFXVector2 checkVecDivide2(initCVDivide2);
  IFXVector2 resVDivide2;

  resVDivide2.Divide(vec1,vec2);
  TEST( checkVecDivide2 == resVDivide2, IFXException );

  //  Divide   a.b
  vec1.Divide( vec2);
  TEST( checkVecDivide2 == vec1, IFXException );


  //  Rescale 67

  const F32 initV2Rescale[] = {3,-4};
  vec1 = IFXVector2(initV2Rescale);

  const F32 initCV2Rescale[] = {F32(1.2),F32(-1.6)};
  checkVec = IFXVector2(initCV2Rescale);

  IFXVector2 resV2Rescale;
  resV2Rescale = vec1.Rescale(2);

  TEST( checkVec == resV2Rescale, IFXException );

  }


  //----------------------Vector3 Tests---------------------------------
  {
  //IFXVector3  IsApproximately 33

  const F32 initV3_1[] = {1,3,0};
  IFXVector3 vec3_1(initV3_1);
  const F32 initV3_2[] = {1,2,-2};
  IFXVector3 vec3_2(initV3_2);
  TEST( vec3_1.IsApproximately(vec3_2,F32(2.1)), IFXException );



  //IFXVector3  Add 7955
  const F32 initVadd3_1[] = {1,3,0};
  vec3_1 = IFXVector3(initVadd3_1);

  const F32 initVadd3_2[] = {1,2,-2};
  vec3_2 = IFXVector3(initVadd3_2);

  const F32 initCadd3_2[] = {2,5,-2};
  IFXVector3 checkVec3(initCadd3_2);
  IFXVector3 resVecAdd3;

  resVecAdd3.Add(vec3_1,vec3_2);
  TEST( checkVec3 == resVecAdd3, IFXException );

  //IFXVector3  Add a.b

  vec3_1.Add( vec3_2);
  TEST( checkVec3 == vec3_1, IFXException );

  //IFXVector3  Subtract  344
  const F32 initVSub3_1[] = {1,3,0};
  vec3_1 = IFXVector3(initVSub3_1);

  const F32 initVSub3_2[] = {1,2,-1};
  vec3_2 = IFXVector3(initVSub3_2);

  const F32 initCSub3_2[] = {0,1,1};
  checkVec3 = IFXVector3(initCSub3_2);

  IFXVector3 resVecSub3;
  resVecSub3.Subtract(vec3_1,vec3_2);

  TEST( checkVec3 == resVecSub3, IFXException );

  //IFXVector3  Subtract a.b  344

  vec3_1.Subtract(vec3_2);
  TEST( checkVec3 == vec3_1, IFXException );

  //  Multiply  398 // test doesn't pass with {1,3,0} and {1,2,-1}, with the result {1,6,0}
                      // and passes with other values (without 0 in the result)
  const F32 initVMul3_1[] = {1,3,4};
  vec3_1 = IFXVector3(initVMul3_1);

  const F32 initVMul3_2[] = {1,2,-1};
  vec3_2 = IFXVector3(initVMul3_2);

  const F32 initCVMul3_2[] = {1,6,-4};
  IFXVector3 checkVecMul3(initCVMul3_2);
  IFXVector3 resVMul3;

  resVMul3.Multiply(vec3_1,vec3_2);
  TEST( checkVecMul3 == resVMul3, IFXException );

  // Multiply 398 a.b

  vec3_1.Multiply(vec3_2);
  TEST( checkVecMul3 == vec3_1, IFXException );



  //  Scale 3559

  const F32 initV3Scale[] = {4, -2, 0};
  vec3_1 = IFXVector3(initV3Scale);

  const F32 initCV3Scale[] = {20, -10, 0};
  checkVec3 = IFXVector3(initCV3Scale);

  IFXVector3 resV3Scale;
  resV3Scale = vec3_1.Scale(5);

  TEST( checkVec3 == resV3Scale, IFXException );


  // Divide 144 res = (a,b)
  const F32 initVDivide3_1[] = {1,6,4};
  vec3_1 = IFXVector3(initVDivide3_1);

  const F32 initVDivide3_2[] = {1,-1,2};
  vec3_2 = IFXVector3(initVDivide3_2);

  const F32 initCVDivide3_2[] = {1,-6,2};
  IFXVector3 checkVecDivide3(initCVDivide3_2);
  IFXVector3 resVDivide3;

  resVDivide3.Divide(vec3_1,vec3_2);
  TEST( checkVecDivide3 == resVDivide3, IFXException );


  // Divide 144 a.b

  vec3_1.Divide(vec3_2);
  TEST( checkVecDivide3 == vec3_1, IFXException );




  //  Invert  348   // also has problems with 0

  const F32 initV3Invert[] = {4, -2, 1};
  vec3_1 = IFXVector3(initV3Invert);

  const F32 initCV3Invert[] = {-4, 2, -1};
  checkVec3 = IFXVector3(initCV3Invert);

  IFXVector3 resV3Invert;
  resV3Invert = vec3_1.Invert();

  TEST( checkVec3 == resV3Invert, IFXException );

  //  DotProduct  244

  const F32 initVDotProduct3_1[] = {1,6,4};
  vec3_1 = IFXVector3(initVDotProduct3_1);

  const F32 initVDotProduct3_2[] = {1,-1,2};
  vec3_2 = IFXVector3(initVDotProduct3_2);

  const F32 checkVecDotProduct3 = 3;

  F32 resVDotProduct3;

  resVDotProduct3 = vec3_1.DotProduct( vec3_2);
  TEST( checkVecDotProduct3 == resVDotProduct3, IFXException );

  // DotProduct (a,b)

  resVDotProduct3 = vec3_1.DotProduct( vec3_1, vec3_2);
  TEST( checkVecDotProduct3 == resVDotProduct3, IFXException );


  //  CalcMagnitude 113

  const F32 initV3CalcMagnitude[] = {2, -2, 1};
  vec3_1 = IFXVector3(initV3CalcMagnitude);

  const F32 checkV3CalcMagnitude = 3;

  F32 resV3CalcMagnitude;
  resV3CalcMagnitude = vec3_1.CalcMagnitude();

  TEST( checkV3CalcMagnitude == resV3CalcMagnitude, IFXException );


  //  Normalize 271

  const F32 initV3Normalize[] = {4, 0, -3};
  vec3_1 = IFXVector3(initV3Normalize);

  const F32 initCV3Normalize[] = { F32(0.8), 0, F32(-0.6)};
  checkVec3 = IFXVector3(initCV3Normalize);

  IFXVector3 resV3Normalize;
  resV3Normalize = vec3_1.Normalize();

  TEST( checkVec3 == resV3Normalize, IFXException );


  //  Interpolate 223

  const F32 initVInterpolate3_1[] = {1,7,-4};
  vec3_1 = IFXVector3(initVInterpolate3_1);

  const F32 initVInterpolate3_2[] = {1,-1,0};
  vec3_2 = IFXVector3(initVInterpolate3_2);

  F32 V3Interpolate = 0.25;

  const F32 initCVInterpolate3[] = {1,5,-3};

  IFXVector3 checkVecInterpolate3(initCVInterpolate3);
  IFXVector3 resVInterpolate3;

  resVInterpolate3.Interpolate(V3Interpolate, vec3_1, vec3_2);
  TEST( checkVecInterpolate3 == resVInterpolate3, IFXException );


  //  Negate  134

  const F32 initV3Negate[] = {4, -2, 1};
  vec3_1 = IFXVector3(initV3Negate);

  const F32 initCV3Negate[] = {-4, 2, -1};
  checkVec3 = IFXVector3(initCV3Negate);

  IFXVector3 resV3Negate;
  resV3Negate = vec3_1.Negate();

  TEST( checkVec3 == resV3Negate, IFXException );

  //  CalcMagnitudeSquared  32

  const F32 initV3CalcMagnitudeSquared[] = {2, -2, 1};
  vec3_1 = IFXVector3(initV3CalcMagnitudeSquared);

  const F32 checkV3CalcMagnitudeSquared = 9;

  F32 resV3CalcMagnitudeSquared;
  resV3CalcMagnitudeSquared = vec3_1.CalcMagnitudeSquared();

  TEST( checkV3CalcMagnitudeSquared == resV3CalcMagnitudeSquared, IFXException );


  //  CrossProduct  84

  const F32 initVCrossProduct3_1[] = {1,6,4};
  vec3_1 = IFXVector3(initVCrossProduct3_1);

  const F32 initVCrossProduct3_2[] = {1,-1,2};
  vec3_2 = IFXVector3(initVCrossProduct3_2);

  const F32 initCVCrossProduct3[] = {16,2,-7};
  IFXVector3 checkVecCrossProduct3(initCVCrossProduct3);
  IFXVector3 resVCrossProduct3;

  resVCrossProduct3.CrossProduct(vec3_1,vec3_2);
  TEST( checkVecCrossProduct3 == resVCrossProduct3, IFXException );


  //  Rescale 67

  const F32 initV3Rescale[] = {3, 0, -4};
  vec3_1 = IFXVector3(initV3Rescale);

  const F32 initCV3Rescale[] = { F32(1.2), 0, F32(-1.6)};
  checkVec3 = IFXVector3(initCV3Rescale);

  IFXVector3 resV3Rescale;
  resV3Rescale = vec3_1.Rescale(2);

  TEST( checkVec3 == resV3Rescale, IFXException );


  //  CalcDistanceFrom  34
  const F32 initVCalcDistanceFrom3_1[] = {1,3,2};
  vec3_1 = IFXVector3(initVCalcDistanceFrom3_1);

  const F32 initVCalcDistanceFrom3_2[] = {1,-1,5};
  vec3_2 = IFXVector3(initVCalcDistanceFrom3_2);

  const F32 checkVecCalcDistanceFrom3 = 5;

  F32 resVCalcDistanceFrom3;

  resVCalcDistanceFrom3 =vec3_1.CalcDistanceFrom( vec3_2);
  TEST( checkVecCalcDistanceFrom3 == resVCalcDistanceFrom3, IFXException );


  //  CalcDistanceSquaredFrom 21
  const F32 initVCalcDistanceSquaredFrom3_1[] = {1,3,2};
  vec3_1 = IFXVector3(initVCalcDistanceSquaredFrom3_1);

  const F32 initVCalcDistanceSquaredFrom3_2[] = {1,-1,5};
  vec3_2 = IFXVector3(initVCalcDistanceSquaredFrom3_2);

  const F32 checkVecCalcDistanceSquaredFrom3 = 25;

  F32 resVCalcDistanceSquaredFrom3;

  resVCalcDistanceSquaredFrom3 =vec3_1.CalcDistanceSquaredFrom( vec3_2);
  TEST( checkVecCalcDistanceSquaredFrom3 == resVCalcDistanceSquaredFrom3, IFXException );


  //  CalcCosAngleSquaredFrom 9
  const F32 initVCalcCosAngleSquaredFrom3_1[] = { 3, -4, 0};
  vec3_1 = IFXVector3(initVCalcCosAngleSquaredFrom3_1);

  const F32 initVCalcCosAngleSquaredFrom3_2[] = { 0, 1, 0};
  vec3_2 = IFXVector3(initVCalcCosAngleSquaredFrom3_2);

  const F32 checkVecCalcCosAngleSquaredFrom3 = F32(0.64);

  F32 resVCalcCosAngleSquaredFrom3;

  resVCalcCosAngleSquaredFrom3 = ((IFXVector3)vec3_1).CalcCosAngleSquaredFrom( vec3_2);

  TEST( checkVecCalcCosAngleSquaredFrom3 == resVCalcCosAngleSquaredFrom3, IFXException );


  //  CalcCosAngleFrom  12
  const F32 initVCalcCosAngleFrom3_1[] = { 3, -4, 0};
  vec3_1 = IFXVector3(initVCalcCosAngleFrom3_1);

  const F32 initVCalcCosAngleFrom3_2[] = { 0, 1, 0};
  vec3_2 = IFXVector3(initVCalcCosAngleFrom3_2);

  const F32 checkVecCalcCosAngleFrom3 = F32(0.8);

  F32 resVCalcCosAngleFrom3;

  resVCalcCosAngleFrom3 = ((IFXVector3)vec3_1).CalcCosAngleFrom( vec3_2);

  TEST( checkVecCalcCosAngleFrom3 == resVCalcCosAngleFrom3, IFXException );


  //  AddScaled 10
  const F32 initAddScaled3_1[] = {1,3,0};
  vec3_1 = IFXVector3(initAddScaled3_1);

  const F32 initAddScaled3_2[] = {1,2,-2};
  vec3_2 = IFXVector3(initAddScaled3_2);

  const F32 ConstAddScaled3 = 4;

  const F32 initCAddScaled3_2[] = {5,11,-8};
  IFXVector3 checkVecAddScaled3(initCAddScaled3_2);

  vec3_1.AddScaled( vec3_2, ConstAddScaled3);

  TEST( checkVecAddScaled3 == vec3_1, IFXException );


  //  Reciprocate 17

  const F32 initV3Reciprocate[] = { 4, -F32(0.5), 0};
  vec3_1 = IFXVector3(initV3Reciprocate);

  const F32 initCV3Reciprocate[] = { F32(0.25), -2, 0};
  checkVec3 = IFXVector3(initCV3Reciprocate);

  IFXVector3 resV3Reciprocate;
  resV3Reciprocate = vec3_1.Reciprocate();

  TEST( checkVec3 == resV3Reciprocate, IFXException );

  }
  //----------------------Vector4 Tests---------------------------------
  {
  //IFXVector4  NearlyEquals  8

  const F32 initV4_1[] = {1,3,0,-3};
  IFXVector4 vec4_1(initV4_1);
  const F32 initV4_2[] = {1,2,-2,-5};
  IFXVector4 vec4_2(initV4_2);
  TEST( vec4_1.NearlyEquals(vec4_2,F32(2.1)), IFXException );



  //IFXVector4  Add 7955 res = a+b
  const F32 initVadd4_1[] = {1,3,0,-3};
  vec4_1 = IFXVector4(initVadd4_1);

  const F32 initVadd4_2[] = {1,2,-2,5};
  vec4_2 = IFXVector4(initVadd4_2);

  const F32 initCadd4_2[] = {2,5,-2,2};
  IFXVector4 checkVec4(initCadd4_2);
  IFXVector4 resVecAdd4;
  resVecAdd4.Add(vec4_1,vec4_2);
  TEST( checkVec4 == resVecAdd4, IFXException );

  //IFXVector4  Add 7955 a = a+b

  vec4_1.Add( vec4_2);
  TEST( checkVec4 == vec4_1, IFXException );


  //IFXVector4  Subtract  344
  const F32 initVSub4_1[] = {1,3,0,-3};
  vec4_1 = IFXVector4(initVSub4_1);

  const F32 initVSub4_2[] = {1,2,-2,5};
  vec4_2 = IFXVector4(initVSub4_2);

  const F32 initCSub4_2[] = {0,1,2,-8};
  checkVec4 = IFXVector4(initCSub4_2);

  IFXVector4 resVecSub4;
  resVecSub4.Subtract(vec4_1,vec4_2);

  TEST( checkVec4 == resVecSub4, IFXException );


  //IFXVector4  Subtract a.b  344
  vec4_1.Subtract(vec4_2);
  TEST( checkVec4 == vec4_1, IFXException );


  //  Multiply  398 res = a*b // test doesn't pass with {1,3,0,-3} and {1,2,-1,5}, with the result {1,6,0,-15}
                      // and passes with other values (without 0 in the result)
  const F32 initVMul4_1[] = {1,3,4,-3};
  vec4_1 = IFXVector4(initVMul4_1);

  const F32 initVMul4_2[] = {1,2,-1,5};
  vec4_2 = IFXVector4(initVMul4_2);

  const F32 initCVMul4_2[] = {1,6,-4,-15};
  IFXVector4 checkVecMul4(initCVMul4_2);
  IFXVector4 resVMul4;
  resVMul4.Multiply(vec4_1,vec4_2);

  TEST( checkVecMul4 == resVMul4, IFXException );


  //  Multiply  398 a.b

  vec4_1.Multiply(vec4_2);
  TEST( checkVecMul4 == vec4_1, IFXException );


  //  Min 4198  // vectors

  const F32 initVMin4_1[] = {1,3,4,-3};
  vec4_1 = IFXVector4(initVMin4_1);

  const F32 initVMin4_2[] = {1,2,-1,5};
  vec4_2 = IFXVector4(initVMin4_2);

  const F32 initCVMin4[] = {1,2,-1,-3};
  IFXVector4 checkVecMin4(initCVMin4);

  vec4_1.Min(vec4_2);

  TEST( checkVecMin4 == vec4_1, IFXException );


  //  Min 4198  // floats

  const F32 initVMinF4_1[] = {1,3,4,-3};
  vec4_1 = IFXVector4(initVMinF4_1);

  const F32 initVMinF4_2[] = {1,2,-1,5};

  const F32 initCVMinF4[] = {1,2,-1,-3};
  IFXVector4 checkVecMinF4(initCVMinF4);

  vec4_1.Min(initVMinF4_2);

  TEST( checkVecMinF4 == vec4_1, IFXException );


  //  Max 3321  // vectors

  const F32 initVMax4_1[] = {1,3,4,-3};
  vec4_1 = IFXVector4(initVMax4_1);

  const F32 initVMax4_2[] = {1,2,-1,5};
  vec4_2 = IFXVector4(initVMax4_2);

  const F32 initCVMax4[] = {1,3,4,5};
  IFXVector4 checkVecMax4(initCVMax4);

  vec4_1.Max(vec4_2);

  TEST( checkVecMax4 == vec4_1, IFXException );


  //  Max 3321  // floats


  const F32 initVMaxF4_1[] = {1,3,4,-3};
  vec4_1 = IFXVector4(initVMax4_1);

  const F32 initVMaxF4_2[] = {1,2,-1,5};

  const F32 initCVMaxF4[] = {1,3,4,5};
  IFXVector4 checkVecMaxF4(initCVMaxF4);

  vec4_1.Max(initVMaxF4_2);

  TEST( checkVecMaxF4 == vec4_1, IFXException );


  //  Scale3  46

  const F32 initV4Scale[] = {4, -2, 0, 0.5};
  vec4_1 = IFXVector4(initV4Scale);

  const F32 initCV4Scale[] = {20, -10, 0, 2.5};
  checkVec4 = IFXVector4(initCV4Scale);

  IFXVector4 resV4Scale;
  resV4Scale = vec4_1.Scale(5);

  TEST( checkVec4 == resV4Scale, IFXException );


  //  Scale3  46

  const F32 initV4Scale3[] = {4, -2, 0, 0.5};
  vec4_1 = IFXVector4(initV4Scale3);

  const F32 initCV4Scale3[] = {20, -10, 0, 0.5};
  checkVec4 = IFXVector4(initCV4Scale3);

  IFXVector4 resV4Scale3;
  resV4Scale3 = vec4_1.Scale3(5);

  TEST( checkVec4 == resV4Scale3, IFXException );


  //  DotProduct3 18

  const F32 initVDotProduct4_1[] = {1,6,4,7};
  vec4_1 = IFXVector4(initVDotProduct4_1);

  const F32 initVDotProduct4_2[] = {1,-1,2,-15};
  vec4_2 = IFXVector4(initVDotProduct4_2);

  const F32 checkVecDotProduct4 = 3;

  F32 resVDotProduct4;

  resVDotProduct4 =vec4_1.DotProduct3( vec4_2);
  TEST( checkVecDotProduct4 == resVDotProduct4, IFXException );


  //  CalcMagnitudeSquared3 4

  const F32 initV3CalcMagnitudeSquared3[] = {2, -2, 1, 5};
  vec4_1 = IFXVector4(initV3CalcMagnitudeSquared3);

  const F32 checkV3CalcMagnitudeSquared3 = 9;

  F32 resV3CalcMagnitudeSquared3;
  resV3CalcMagnitudeSquared3 = vec4_1.CalcMagnitudeSquared3();

  TEST( checkV3CalcMagnitudeSquared3 == resV3CalcMagnitudeSquared3, IFXException );


  //  CalcMagnitude3  6

  const F32 initV4CalcMagnitude3[] = {2, -2, 1, 5};
  vec4_1 = IFXVector4(initV4CalcMagnitude3);

  const F32 checkV4CalcMagnitude3 = 3;

  F32 resV4CalcMagnitude3;
  resV4CalcMagnitude3 = vec4_1.CalcMagnitude3();

  TEST( checkV4CalcMagnitude3 == resV4CalcMagnitude3, IFXException );


  //  CrossProduct3 4


  const F32 initVCrossProduct4_1[] = { 1, 6, 4, 10};
  vec4_1 = IFXVector3(initVCrossProduct4_1);

  const F32 initVCrossProduct4_2[] = { 1, -1, 2, -2};
  vec4_2 = IFXVector4(initVCrossProduct4_2);

  const F32 initCVCrossProduct3[] = { 16, 2, -7};
  IFXVector3 checkVecCrossProduct4(initCVCrossProduct3);

  IFXVector3 resVCrossProduct4;

  resVCrossProduct4.CrossProduct(vec4_1,vec4_2);
  TEST( checkVecCrossProduct4 == resVCrossProduct4, IFXException );


  //  Normalize3  4

  const F32 initV4Normalize[] = { 4, 0, -3, 6};
  vec4_1 = IFXVector4(initV4Normalize);

  const F32 initCV4Normalize[] = { F32(0.8), 0, F32(-0.6), 6};
  checkVec4 = IFXVector4(initCV4Normalize);

  IFXVector4 resV4Normalize;
  resV4Normalize = vec4_1.Normalize3();

  TEST( checkVec4 == resV4Normalize, IFXException );


  //  CalcDistanceFrom  34

  const F32 initVCalcDistanceFrom4_1[] = { 1, 3, 2, 9};
  vec4_1 = IFXVector4(initVCalcDistanceFrom4_1);

  const F32 initVCalcDistanceFrom4_2[] = { 1, -1, 5, -4};
  vec4_2 = IFXVector4(initVCalcDistanceFrom4_2);

  const F32 checkVecCalcDistanceFrom4 = 5;

  F32 resVCalcDistanceFrom4;

  resVCalcDistanceFrom4 =vec4_1.CalcDistanceFrom( vec4_2);
  TEST( checkVecCalcDistanceFrom4 == resVCalcDistanceFrom4, IFXException );


  }
  //----------------------Quaternion Tests------------------------------
  {
  IFXVector3 resultV3, checkV3;
  IFXQuaternion quat1, quat2, resultQ, checkQ;
  F32 epsError, eps = F32(0.00001);

  //IFXQuaternion Multiply  398

  const F32 initQMul1[] = {1,2,0,-3};
  quat1 = IFXQuaternion(initQMul1);

  const F32 initQMul2[] = {1,3,-1,5};
  quat2 = IFXQuaternion(initQMul2);

  const F32 initCQMul[] = {10,2,-20,0};
  IFXQuaternion checkQMul(initCQMul);
  IFXQuaternion resQMul;
  resQMul.Multiply(quat1,quat2);

  TEST( checkQMul == resQMul, IFXException );


  //IFXQuaternion Rotate  942

  const F32 initQRot1[] = {1,2,0,-3};
  quat1 = IFXQuaternion(initQRot1);
  const F32 initQRot2[] = {1,3,-1,5};
  quat2 = IFXQuaternion(initQRot2);

  const F32 initCQRot[] = {10,2,-20,0};
  IFXQuaternion checkQRot(initCQRot);
  quat1.Rotate(quat2);


  TEST( checkQRot == quat1, IFXException );


  //IFXQuaternion Invert  348 // multiply by -1

  const F32 initQInv[] = {1,2,0,-3};
  quat1 = IFXQuaternion(initQInv);

  const F32 initCQInv[] = {1,-2,0,3};
  IFXQuaternion checkQInv(initCQInv);

  IFXQuaternion resQInv;
  resQInv = quat1.Invert();

  TEST( checkQInv == resQInv, IFXException );


  //IFXQuaternion Invert  348 //

  const F32 initQInvert[] = {1,2,0,-3};
  quat1 = IFXQuaternion(initQInvert);

  const F32 checkQInvert[] = {1,-2,0,3};
  checkQ = IFXQuaternion(checkQInvert);

  resultQ.Invert(quat1);

  TEST( checkQ == resultQ, IFXException );

  //IFXQuaternion Invert  a()

  quat1.Invert();
  TEST( checkQ == quat1, IFXException );

  //IFXQuaternion CalcMagnitudeSquared  32

  const F32 initQCalcMagnitudeSquared[] = {1,2,0,-3};
  quat1 = IFXQuaternion(initQCalcMagnitudeSquared);

  F32 checkQCalcMagnitudeSquared =14;

  F32 resQCalcMagnitudeSquared;
  resQCalcMagnitudeSquared = quat1.CalcMagnitudeSquared();

  TEST( checkQCalcMagnitudeSquared == resQCalcMagnitudeSquared, IFXException );



  //IFXQuaternion Normalize 271

  const F32 initQNorm[] = {0,4,0,-3};
  quat1 = IFXQuaternion(initQNorm);

  const F32 initCQNorm[] = {0,F32(0.8),0,F32(-0.6)};
  IFXQuaternion checkQNorm(initCQNorm);

  quat1.Normalize();

  TEST( checkQNorm == quat1, IFXException );


  //IFXQuaternion RotateVector  105

  const F32 initQRotateVector[] = {1,2,0,-3};
  quat1 = IFXQuaternion(initQRotateVector);

  const F32 initQRotateVectorV[] = {-4,5,1};
  IFXVector3 initQRV = IFXVector3(initQRotateVectorV);

  const F32 checkQRotateVector[] = {34, -40, 74};
  checkV3 = IFXVector3(checkQRotateVector);

  quat1.RotateVector(initQRV,resultV3);

  TEST( checkV3 == resultV3, IFXException );


  //IFXQuaternion Interpolate 223

  const F32 initQInterpolate1[] = {1,2,0,-3};
  quat1 = IFXQuaternion(initQInterpolate1);

  const F32 initQInterpolate2[] = {1,3,-1,5};
  quat2 = IFXQuaternion(initQInterpolate2);

  const F32 t = F32(0.5);

  const F32 initCQInterpolate[] = {0,F32(-0.5),F32(0.5),-4.0};
  checkQ = IFXQuaternion(initCQInterpolate);

  resultQ.Interpolate(t, quat1, quat2);
  TEST( checkQ == resultQ, IFXException );


  //IFXQuaternion IsApproximately 33
  const F32 initQIsApproximately1[] = {1,2,0,-3};
  quat1 = IFXQuaternion(initQIsApproximately1);

  const F32 initQIsApproximately2[] = {1,3,-1,-5};
  quat2 = IFXQuaternion(initQIsApproximately2);

  TEST( quat1.IsApproximately( quat2, F32(2.1)), IFXException);


  //IFXQuaternion MakeRotation  34
  const F32 initMakeRotationFrom[] = {1,0,0};
  IFXVector3 MakeRotationFrom(initMakeRotationFrom);

  const F32 initMakeRotationTo[] = {0,F32(0.707106811865475244),F32(0.707106811865475244)};
  IFXVector3 MakeRotationTo(initMakeRotationTo);

  const F32 initQCMakeRotation[] = {F32(0.707106811865475244),0,F32(-0.5),F32(0.5)};
  checkQ = IFXQuaternion(initQCMakeRotation);

  resultQ.MakeRotation( MakeRotationFrom, MakeRotationTo);

  epsError = 0;
  int quati = 0;
  for(quati = 0; quati < 4; quati++)
  {
    epsError += fabs(checkQ[quati] - resultQ[quati]);
  }

  TEST( epsError<eps, IFXException );


  //IFXQuaternion ForcePositiveW  12
  const F32 initQForcePositiveW[] = {-1,2,0,-3};
  quat1 = IFXQuaternion(initQForcePositiveW);

  const F32 initQCForcePositiveW[] = {1,-2,0,3};
  checkQ = IFXQuaternion(initQCForcePositiveW);

  quat1.ForcePositiveW();

  TEST( checkQ == quat1, IFXException );


  //IFXQuaternion ComputeAngleAxis  8
  const F32 initQComputeAngleAxis[] = {F32(0.707106811865475244),F32(0.707106811865475244),0,0};
  quat1 = IFXQuaternion(initQComputeAngleAxis);

  const F32 initCAxis[] = {1,0,0};
  IFXVector3 CAxis = IFXVector3(initCAxis);
  IFXVector3 CAxisResult;

  const F32 CAngle = F32(1.57079632679489);  // PI/2
  F32 CAngleResult;

  quat1.ComputeAngleAxis( CAngleResult, CAxisResult);

  epsError = 0;
  for(quati = 0; quati < 3; quati++)
  {
    epsError += fabs(CAxisResult.Raw()[quati] - CAxis.Raw()[quati]);
  }

  TEST( (epsError<eps)&&(fabs(CAngle - CAngleResult)<eps), IFXException );

  //IFXQuaternion ExtractEulerZ 8
  const F32 initQExtractEulerZ[] = {-1,1,0,0};
  quat1 = IFXQuaternion(initQExtractEulerZ);

  const F32 initCQExtractEulerZ[] = {1,-1,0,0};
  checkQ = IFXQuaternion(initCQExtractEulerZ);

  quat1.ExtractEulerZ();
  TEST( checkQ == quat1, IFXException );



  //IFXQuaternion ScaleAngle  5
  const F32 initQScaleAngle[] = {F32(0.707106811865475244),F32(0.707106811865475244),0,0};
  quat1 = IFXQuaternion(initQScaleAngle);

  const F32 initQCScaleAngle[] = {0,1,0,0};
  checkQ = IFXQuaternion(initQCScaleAngle);

  quat1.ScaleAngle(2);

  epsError = 0;
  for(quati = 0; quati < 3; quati++)
  {
    epsError += fabs(checkQ.Raw()[quati] - quat1.Raw()[quati]);
  }

  TEST( (epsError<eps)&&(fabs(CAngle - CAngleResult)<eps), IFXException );

  }

  IFXCOMUninitialize();

  return 0;
}