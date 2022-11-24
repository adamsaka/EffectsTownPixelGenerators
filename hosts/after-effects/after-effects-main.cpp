/********************************************************************************************************
The main entry for point for After Effects Plug-ins

This is a .dll (renamed .aex) 

Author:			(c) 2022 Maths Town

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
********************************************************************************************************/

//Plug name and version information.
//**WARNING**: ALSO update these constants in AE.r.  They must match.
#define PluginName					"Watercolour Texture"
#define PluginMenu					"Effects Town"
#define PluginIdentifier			"Town.Effects.WatercolourTexture"
#define	PluginMajorVersion			1
#define	PluginMinorVersion			0
#define	PluginBugVersion			0
#define	PluginBuildVersion			1


#include "ae.h"


#include <string>
#include <sstream>


static void GlobalSetup(const PF_InData* in_data, PF_OutData* out_data) noexcept;
static void About(const PF_InData* in_data, PF_OutData* out_data);

/*******************************************************************************************************
Global Thread-local Variables
These variables are updated each time the plug-in is called.
They are needed to access After Effects Callbacks, and it is often inconvient to pass them around.
*******************************************************************************************************/
thread_local PF_InData* globalTL_in_data{ nullptr };

/*******************************************************************************************************
DLL Entry function called by After Effects when the plugin is loaded.
Note: Any changes should have a matching change in (.r) resource file.
(May not be called for older versions of AE).
*******************************************************************************************************/
extern "C" DllExport PF_Err PluginDataEntryFunction(PF_PluginDataPtr inPtr, PF_PluginDataCB inPluginDataCallBackPtr, SPBasicSuite * inSPBasicSuitePtr, const char* inHostName, const char* inHostVersion) {
	PF_Err result = PF_Err_INVALID_CALLBACK;
	result = PF_REGISTER_EFFECT(inPtr, inPluginDataCallBackPtr, PluginName, PluginIdentifier, PluginMenu, AE_RESERVED_INFO);
	return result;
}

#pragma warning(suppress:26487)
/*******************************************************************************************************
The After Effects plugin's main entry point.
*******************************************************************************************************/
PF_Err EffectMain(PF_Cmd cmd, PF_InData* in_data, PF_OutData* out_data, PF_ParamDef* params[], PF_LayerDef* output, void* extra) {
	globalTL_in_data = in_data;
	PF_Err err = PF_Err_NONE;
	try {
		

		switch (cmd) {
		case PF_Cmd_ABOUT:
			About(in_data, out_data);
			break;

		case PF_Cmd_GLOBAL_SETUP:
			GlobalSetup(in_data, out_data);
			break;

		case PF_Cmd_GLOBAL_SETDOWN:
			break;

		case PF_Cmd_PARAMS_SETUP:
			
			break;

		case PF_Cmd_USER_CHANGED_PARAM:
			
			break;

		case PF_Cmd_SEQUENCE_SETUP:			
			break;

		case PF_Cmd_SEQUENCE_RESETUP:
			break;

		case PF_Cmd_SEQUENCE_FLATTEN:
			break;

		case PF_Cmd_SEQUENCE_SETDOWN:
			break;

		case PF_Cmd_RENDER:
			
			break;

		case PF_Cmd_SMART_PRE_RENDER:
			
			break;

		case PF_Cmd_SMART_RENDER:
			
			break;
		default:
			break;
		}
		
	}

	//Allow throwing AE error codes as exceptions. 
	//Note: All error codes genenerated by internally by After Effects MUST be returned.  We check and throw them.
	catch (const PF_Err & thrown_err) {
		err = thrown_err;
	}
	
	//AE won't handle exceptions, we need to catch them here, and display an error message.
	catch (const std::exception ex) {
		std::ostringstream ss;
		ss << "Error in \"" << PluginName << "\"\n\n" << ex.what();
		strncpy_s(out_data->return_msg, ss.str().c_str(), 256);
		out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
	}

	globalTL_in_data = nullptr;
	return err;
}

/*******************************************************************************************************
GlobalSetup
Note: Changes to flags should have a matching change in (.r) resource file.
*******************************************************************************************************/
static void GlobalSetup(const PF_InData* in_data, PF_OutData* out_data) noexcept{
	if (!in_data || !out_data) return;
	out_data->my_version = PF_VERSION(PluginMajorVersion, PluginMinorVersion, PluginBugVersion, PF_Stage_DEVELOP, PluginBuildVersion);
	out_data->out_flags = PF_OutFlag_DEEP_COLOR_AWARE | PF_OutFlag_SEQUENCE_DATA_NEEDS_FLATTENING | PF_OutFlag_PIX_INDEPENDENT;
	out_data->out_flags2 = PF_OutFlag2_SUPPORTS_SMART_RENDER | PF_OutFlag2_FLOAT_COLOR_AWARE | PF_OutFlag2_SUPPORTS_THREADED_RENDERING | PF_OutFlag2_SUPPORTS_GET_FLATTENED_SEQUENCE_DATA;
}



/*******************************************************************************************************
About dialog to be displayed.
Text to be displayed in out_data->return_msg
*******************************************************************************************************/
static void About(const PF_InData* in_data, PF_OutData* out_data) {
	if (!in_data || !out_data) return;
	std::ostringstream str;
	str << PluginName << " v" << PluginMajorVersion << "." << PluginMinorVersion;
	strncpy_s(out_data->return_msg, str.str().c_str(), sizeof(out_data->return_msg));
	
}
