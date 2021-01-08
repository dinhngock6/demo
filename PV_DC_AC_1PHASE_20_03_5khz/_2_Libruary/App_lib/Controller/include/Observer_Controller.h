/*
 * Observer_Controller.h
 *
 *  Created on: 1 Apr 2020
 *      Author: Dinh Ngoc
 */

#ifndef _OBSERVER_CONTROLLER_H_
#define _OBSERVER_CONTROLLER_H_

#include "CLAmath.h"

typedef struct
{
    float Lambda;
    float L1_00;
    float L1_01;
    float L1_10;
    float L1_11;

    float L2_00;
    float L2_01;
    float L2_10;
    float L2_11;

    float Phi_00;
    float Phi_01;
    float Phi_10;
    float Phi_11;

    float Gamma_00;
    float Gamma_10;

}OS_Parameter;

typedef struct
{
    // Input
    float iL;
    float Vo;
    float Vo_ref_k_1;
    float Vo_ref_k;
    float PWM;

    // Ouput
    float d;
    float obF_k0;
    float obX_k0;
    float obF_k1;
    float obX_k1;
    float Iref;

    // Var
    float obF1_k_1;
    float obF2_k_1;
    float obVo_k_1;
    float obIL_k_1;

    float Ui_k_1;
    float Vo_k_1;
    float iL_k_1;

    float iLk1_ref;
    float ConstR;
    float Ui;
    float Dtemp;
    float erX1_k;
    float erX2_k;
}OS_Calculator;

#define OS_PARAMETER_INT(v)     \
v.Lambda = 0;   \
v.L1_00 = 0;    \
v.L1_01 = 0;    \
v.L1_10 = 0;    \
v.L1_11 = 0;    \
v.L2_00 = 0;    \
v.L2_01 = 0;    \
v.L2_10 = 0;    \
v.L2_11 = 0;    \
v.Phi_00 = 0;    \
v.Phi_01 = 0;    \
v.Phi_10 = 0;    \
v.Phi_11 = 0;    \
v.Gamma_00 = 0;    \
v.Gamma_10 = 0;

#define OS_CALCULATOR_INT(v)        \
v.iL = 0;           \
v.Vo = 0;           \
v.Vo_ref_k_1 = 0;   \
v.Vo_ref_k = 0;     \
v.PWM = 0;          \
                    \
v.d = 0;            \
v.obF_k0 = 0;        \
v.obX_k0 = 0;        \
v.obF_k1 = 0;        \
v.obX_k1 = 0;        \
v.Iref = 0;         \
v.obF1_k_1 = 0;     \
v.obF2_k_1 = 0;     \
v.obVo_k_1 = 0;     \
v.obIL_k_1 = 0;     \
                    \
v.Ui_k_1 = 0;       \
v.Vo_k_1 = 0;       \
v.iL_k_1 = 0;       \
                    \
v.iLk1_ref = 0;    \
v.ConstR = 0;       \
v.Ui = 0;           \
v.Dtemp = 0;        \
v.erX1_k = 0;       \
v.erX2_k = 0;

#define OS_CALCULATOR_MARCO(v,v2)                                                                           \
v.ConstR = v2.Gamma_10/v2.Gamma_00;                                                                         \
v.iLk1_ref = v.ConstR*(v.Vo_ref_k_1 + v2.Lambda*v.Vo_ref_k)+ (v2.Phi_11 - v.ConstR*v2.Phi_01)*v.iL         \
        + (v2.Phi_10 - v.ConstR*(v2.Phi_00 + v2.Lambda))*v.Vo + v.obF_k1 - v.ConstR*v.obF_k0;              \
if(v.iLk1_ref > 100)v.iLk1_ref = 100.0;                                                                     \
if(v.iLk1_ref < -100.0)v.iLk1_ref = -100.0;                                                                 \
v.Ui = (1/v2.Gamma_10)*(v.iLk1_ref - v2.Phi_10*v.Vo - v2.Phi_11*v.iL - v.obF_k1);                           \
if(v.Ui > 1.0)v.Ui = 1.0;                                                                                   \
if(v.Ui < -1.0)v.Ui = -1.0;                                                                                 \
v.d = v.Ui;                                                                                                 \
v.erX1_k = v.Vo_k_1 - v.obVo_k_1;                                                                           \
v.erX2_k = v.iL_k_1 - v.obIL_k_1;                                                                           \
v.obX_k0 = v2.Phi_00*v.obVo_k_1 + v2.Phi_01*v.obIL_k_1 + v2.Gamma_00*v.Ui_k_1 + v2.L1_00*v.erX1_k + v2.L1_01*v.erX2_k +v.obF1_k_1;\
v.obX_k1 = v2.Phi_10*v.obVo_k_1 + v2.Phi_11*v.obIL_k_1 + v2.Gamma_10*v.Ui_k_1 + v2.L1_10*v.erX1_k + v2.L1_11*v.erX2_k +v.obF2_k_1;\
v.obF_k0 = v.obF1_k_1 + v2.L2_00*v.erX1_k + v2.L2_01*v.erX2_k;                                              \
v.obF_k1 = v.obF2_k_1 + v2.L2_10*v.erX1_k + v2.L2_11*v.erX2_k;                                              \
                                                                                                        \
v.obF1_k_1 = v.obF_k0;                                                                                  \
v.obF2_k_1 = v.obF_k1;                                                                                  \
v.obVo_k_1 = v.obX_k0;                                                                                  \
v.obIL_k_1 = v.obX_k1;                                                                                  \
v.Ui_k_1 = v.d;                                                                                         \
v.Vo_k_1 = v.Vo;                                                                                        \
v.iL_k_1 = v.iL;                                                                                        \
v.Vo_ref_k_1 = v. Vo_ref_k;





#endif /*_OBSERVER_CONTROLLER_H_ */
