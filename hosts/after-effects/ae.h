/********************************************************************************************************

Authors:		(c) 2022 Maths Town

Licence:		The GNU General Public License Version 3

*********************************************************************************************************
This program is free software: you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************

Description:

A 'meta header' to include all required After Effects SDK headers.


*******************************************************************************************************/
#pragma once


/***********************************************************************************************************
Project Configuration Notes :

A Visual Studio project should have the following set to complile After Effects Effects SDK.

	"Additional Include Directories": ../../3rd-party/AfterEffectsSDK/Examples/Headers/;../../3rd-party/AfterEffectsSDK/Examples/Headers/SP;../../3rd-party/AfterEffectsSDK/Examples/Util;
	"Preprocessor Definitions": MSWindows;WIN32;_WINDOWS;

	"Struct Member Alignment": default
	(Not sure why examples default to something else, but it caused failures for me if I follow the examples)

	"Target File Extension": .aex

Setup a custom build tool for PIPL resource file "ae.r"  (put in project directory):

	"Command": 
	cl /I "$(ProjectDir)..\..\3rd-party\AfterEffectsSDK\Examples\Headers" /EP "%(Filename).r" > "$(IntDir)"\\"%(Filename).rr"
	"$(ProjectDir)..\..\3rd-party\AfterEffectsSDK\Examples\Resources\PiPLTool" "$(IntDir)%(Filename).rr" "$(IntDir)%(Filename).rrc"
	cl /D "MSWindows" /EP $(IntDir)%(Filename).rrc >               "$(ProjectDir)"\\"%(Filename)".rc

	"Outputs":
	$(ProjectDir)%(Filename).rc;

Include the output ae.rc in project

**********************************************************************************************************/

typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned short		u_int16;
typedef unsigned long		u_long;
typedef short int			int16;

#define PF_TABLE_BITS	12
#define PF_TABLE_SZ_16	4096
#define PF_DEEP_COLOR_AWARE 1	// make sure we get 16bpc pixels; 

//Supress code warning in included headers
#include <codeanalysis/warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)

//These includes are from the After Effects SDK
#include "AEConfig.h"
#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "AE_EffectCBSuites.h"
#include "String_Utils.h"
#include "AE_GeneralPlug.h"
#include "AEFX_ChannelDepthTpl.h"
#include "AEGP_SuiteHandler.h"

#pragma warning(pop)

//Our globally accesible variables.
extern thread_local PF_InData* globalTL_in_data;

//A helper function that checks for an error and throws if there is one.
//Note: It is very important to ensure that all AE errors get back to After Effects, and they occur often, because it includes UI stopping render.
inline void CheckErrorAE(PF_Err err) {
	if (err != PF_Err_NONE) throw (err);
}

//Main Entry Point
extern "C" {
	DllExport PF_Err EffectMain(PF_Cmd cmd, PF_InData* in_data, PF_OutData* out_data, PF_ParamDef* params[], PF_LayerDef* output, void* extra);
}
