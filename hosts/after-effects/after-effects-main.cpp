/********************************************************************************************************

Authors:		(c) 2022 Maths Town

Licence:		The MIT License

*********************************************************************************************************
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
********************************************************************************************************

Description:

	The main entry for point for After Effects Plug-ins
	This is a .dll (renamed .aex) 
********************************************************************************************************/


//Set project include directories to direct to the correct project
//Plug name and version information stored in config.h
#include "config.h"  
#include "parameters.h"

//After Effects SDK 'meta-header'
#include "after-effects-sdk.h"
#include "after-effect-parameter-helper.h"
#include "after-effects-render.h"

#include <string>
#include <sstream>


static void after_effects_global_setup(const PF_InData* in_data, PF_OutData* out_data) noexcept;
static void after_effects_about(const PF_InData* in_data, PF_OutData* out_data);
static void after_effects_parameter_setup(PF_OutData* out_data);


//We store a local threadlocal copy of in_data because it may be needed in places like destructors, and it is messy to pass it around everywhere.
thread_local PF_InData* globalTL_in_data{ nullptr };


/*******************************************************************************************************
DLL Entry function called by After Effects when the plugin is loaded.
Note: Any changes should have a matching change in (.r) resource file.
(May not be called for older versions of AE).
*******************************************************************************************************/
extern "C" DllExport PF_Err PluginDataEntryFunction(PF_PluginDataPtr inPtr, PF_PluginDataCB inPluginDataCallBackPtr, [[maybe_unused]] SPBasicSuite * inSPBasicSuitePtr, [[maybe_unused]] const char* inHostName, [[maybe_unused]]  const char* inHostVersion) {
	PF_Err result = PF_Err_INVALID_CALLBACK;
	result = PF_REGISTER_EFFECT(inPtr, inPluginDataCallBackPtr, PluginName, PluginIdentifier, PluginMenu, AE_RESERVED_INFO);
	return result;
}


/*******************************************************************************************************
The After Effects plugin's main entry point.
*******************************************************************************************************/
PF_Err EffectMain(PF_Cmd cmd, PF_InData* in_data, PF_OutData* out_data,  PF_ParamDef* params[], PF_LayerDef* output,  void* extra) {
	globalTL_in_data = in_data;
	PF_Err err = PF_Err_NONE;
	try {
		

		switch (cmd) {
		case PF_Cmd_ABOUT:
			after_effects_about(in_data, out_data);
			break;

		case PF_Cmd_GLOBAL_SETUP:
			after_effects_global_setup(in_data, out_data);
			break;

		case PF_Cmd_GLOBAL_SETDOWN:
			break;

		case PF_Cmd_PARAMS_SETUP:
			after_effects_parameter_setup(out_data);
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

		case PF_Cmd_SMART_PRE_RENDER:
			after_effects_smart_pre_render(in_data, static_cast<PF_PreRenderExtra*>(extra));
			break;

		case PF_Cmd_SMART_RENDER:
			after_effects_smart_render(in_data, out_data, static_cast<PF_SmartRenderExtra*>(extra));
			break;
		case PF_Cmd_RENDER:
			after_effects_non_smart_render(in_data, out_data, params, output);
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
static void after_effects_global_setup(const PF_InData* in_data, PF_OutData* out_data) noexcept{
	if (!in_data || !out_data) return;
	out_data->my_version = PF_VERSION(PluginMajorVersion, PluginMinorVersion, PluginBugVersion, PF_Stage_DEVELOP, PluginBuildVersion);
	out_data->out_flags = PF_OutFlag_DEEP_COLOR_AWARE | PF_OutFlag_SEQUENCE_DATA_NEEDS_FLATTENING | PF_OutFlag_PIX_INDEPENDENT;
	out_data->out_flags2 = PF_OutFlag2_SUPPORTS_SMART_RENDER | PF_OutFlag2_FLOAT_COLOR_AWARE | PF_OutFlag2_SUPPORTS_THREADED_RENDERING | PF_OutFlag2_SUPPORTS_GET_FLATTENED_SEQUENCE_DATA;
}



/*******************************************************************************************************
About dialog to be displayed.
Text to be displayed in out_data->return_msg
*******************************************************************************************************/
static void after_effects_about(const PF_InData* in_data, PF_OutData* out_data) {
	if (!in_data || !out_data) return;
	std::ostringstream str;
	str << PluginName << " v" << PluginMajorVersion << "." << PluginMinorVersion;
	strncpy_s(out_data->return_msg, str.str().c_str(), sizeof(out_data->return_msg));
	
}


/*******************************************************************************************************
ParameterSetup event.
Inform AE of our parameter list.
*******************************************************************************************************/
static void after_effects_parameter_setup(PF_OutData* out_data) {
	//Get parameters from project
	auto params = build_project_parameters();

	//Build After Effect Parameters
	for (auto p : params.entries) {
		switch (p.type) {
		case ParameterType::seed:
			ParameterHelper::AddSlider(p.id, p.name, static_cast<float>(p.min), static_cast<float>(p.max), static_cast<float>(p.slider_min), static_cast<float>(p.slider_max), static_cast<float>(p.initial_value), 0);
			break;
		case ParameterType::number:
			ParameterHelper::AddSlider(p.id, p.name, static_cast<float>(p.min), static_cast<float>(p.max), static_cast<float>(p.slider_min), static_cast<float>(p.slider_max), static_cast<float>(p.initial_value), p.precision);
			break;

		default:
			break;
		}
	}


	out_data->num_params = ParameterHelper::GetNumberParameters();
}