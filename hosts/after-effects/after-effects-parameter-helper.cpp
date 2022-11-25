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

	A helper class for managing After Effects Parameters.

*******************************************************************************************************/
#include "after-effect-parameter-helper.h"



/*******************************************************************************************************
Add a checkbox parameter
(Only to be called during the PF_Cmd_PARAMS_SETUP event)
*******************************************************************************************************/
void ParameterHelper::AddCheckBox(ParameterID id, const std::string& name, const std::string& comment, bool value)
{
	PF_ParamDef	def{};
	def.param_type = PF_Param_CHECKBOX;
#pragma warning(suppress:26485) 
	strncpy_s(def.name, name.c_str(), sizeof(def.name)); //AE only has 32 bytes for a string.  Safe copy prevents overrun.
	def.name[sizeof(def.name) - 1] = 0; //Ensure string is null terminated
	def.uu.id = static_cast<A_long>(id);
	def.flags = 0;
	def.ui_flags = 0;
	def.u.bd.u.nameptr = comment.c_str();
	def.u.bd.dephault = value;
	def.u.bd.value = value;
	const auto err = globalTL_in_data->inter.add_param(globalTL_in_data->effect_ref, -1, &def);
	if (err) throw(err);
	addToLookup(id);
}

/*******************************************************************************************************
Add a button parameter
(Only to be called during the PF_Cmd_PARAMS_SETUP event)
*******************************************************************************************************/
void ParameterHelper::AddButton(ParameterID id, const std::string& name, const std::string& buttonText) {
	PF_ParamDef	def{};	//Must be zero initialised.
	def.param_type = PF_Param_BUTTON;
#pragma warning(suppress:26485)
	strncpy_s(def.name, name.c_str(), sizeof(def.name)); //AE only has 32 bytes for a string.  Safe copy prevents overrun.
	def.name[sizeof(def.name) - 1] = 0; //Ensure string is null terminated
	def.uu.id = static_cast<long>(id);
	def.flags = PF_ParamFlag_SUPERVISE; //buttons should be supervised
	def.ui_flags = 0;
	def.u.button_d.u.namesptr = buttonText.c_str();
	const auto err = globalTL_in_data->inter.add_param(globalTL_in_data->effect_ref, -1, &def);
	if (err) throw(err);
	addToLookup(id);
}

/*******************************************************************************************************
Create a slider parameter
(Only to be called during the PF_Cmd_PARAMS_SETUP event)
*******************************************************************************************************/
void ParameterHelper::AddSlider(ParameterID id, const std::string& name, float min, float max, float sliderMin, float sliderMax, float value, short precision) {
	PF_ParamDef	def{};	//Must be zero initialised.
	def.param_type = PF_Param_FLOAT_SLIDER;
#pragma warning(suppress:26485)
	strncpy_s(def.name, name.c_str(), sizeof(def.name)); //AE only has 32 bytes for a string.  Safe copy prevents overrun.
	def.name[sizeof(def.name) - 1] = 0; //Ensure string is null terminated
	def.uu.id = static_cast<long>(id);
	def.flags = 0;
	def.ui_flags = 0;
	def.u.fs_d.valid_min = min;   //minimum value of input
	def.u.fs_d.valid_max = max;   //maximum value of input
	def.u.fs_d.slider_min = sliderMin;  //minimum value of slider control (value can be less)
	def.u.fs_d.slider_max = sliderMax;  //maximum value of slider control (value can be higher)
	def.u.fs_d.value = value;
	def.u.fs_d.dephault = value;
	def.u.fs_d.precision = precision;
	const auto err = globalTL_in_data->inter.add_param(globalTL_in_data->effect_ref, -1, &def);
	if (err) throw(err);
	addToLookup(id);

}

/*******************************************************************************************************
Create a DropDown (Popup) parameter
First value is 1 (not zero).
(Only to be called during the PF_Cmd_PARAMS_SETUP event)
*******************************************************************************************************/
void ParameterHelper::AddList(ParameterID id, const std::string& name, const std::string& choices, short value, bool supervise) {
	PF_ParamDef	def{};	//Must be zero initialised.
	short choiceCount = 1;
	for (int i = 0; i < choices.length(); i++) if (choices.at(i) == '|') choiceCount++;
	def.param_type = PF_Param_POPUP;
#pragma warning(suppress:26485)
	strncpy_s(def.name, name.c_str(), sizeof(def.name)); //AE only has 32 bytes for a string.  Safe copy prevents overrun.
	def.name[sizeof(def.name) - 1] = 0; //Ensure string is null terminated
	def.flags = (supervise) ? PF_ParamFlag_SUPERVISE : 0;
	def.ui_flags = 0;
	def.uu.id = static_cast<long>(id);
	def.u.pd.dephault = value;
	def.u.pd.value = value; //1 based index
	def.u.pd.num_choices = choiceCount;
	def.u.pd.u.namesptr = choices.c_str();
	const auto err = globalTL_in_data->inter.add_param(globalTL_in_data->effect_ref, -1, &def);
	if (err) throw(err);
	addToLookup(id);

}
/*******************************************************************************************************
Create a Group Start parameter
(Only to be called during the PF_Cmd_PARAMS_SETUP event)
*******************************************************************************************************/
void ParameterHelper::AddGroupStart(ParameterID id, const std::string name) {
	PF_ParamDef	def{};	//Must be zero initialised.
	def.param_type = PF_Param_GROUP_START;
#pragma warning(suppress:26485)
	strncpy_s(def.name, name.c_str(), sizeof(def.name)); //AE only has 32 bytes for a string.  Safe copy prevents overrun.
	def.name[sizeof(def.name) - 1] = 0; //Ensure string is null terminated
	def.uu.id = static_cast<long>(id);
	def.flags = 0;
	def.ui_flags = 0;
	const auto err = globalTL_in_data->inter.add_param(globalTL_in_data->effect_ref, -1, &def);
	if (err) throw(err);
	addToLookup(id);
}

/*******************************************************************************************************
Create a Group End parameter
(Only to be called during the PF_Cmd_PARAMS_SETUP event)
*******************************************************************************************************/
void ParameterHelper::AddGroupEnd(ParameterID id) {
	PF_ParamDef	def{};	//Must be zero initialised.
	def.param_type = PF_Param_GROUP_END;
	def.uu.id = static_cast<long>(id);
	const auto err = globalTL_in_data->inter.add_param(globalTL_in_data->effect_ref, -1, &def);
	if (err) throw(err);
	addToLookup(id);
}

/*******************************************************************************************************
Create a "Colour picker" parameter
(Only to be called during the PF_Cmd_PARAMS_SETUP event)
*******************************************************************************************************/
void ParameterHelper::AddColourPickerRGB(ParameterID id, const std::string& name, unsigned char red, unsigned char green, unsigned char blue) {
	PF_ParamDef	def{};
	def.param_type = PF_Param_COLOR;
#pragma warning(suppress:26485)
	strncpy_s(def.name, name.c_str(), sizeof(def.name)); //AE only has 32 bytes for a string.  Safe copy prevents overrun.
	def.name[sizeof(def.name) - 1] = 0; //Ensure string is null terminated
	def.uu.id = static_cast<long>(id);
	def.flags = 0;
	def.ui_flags = 0;
	def.u.cd.value.red = red;
	def.u.cd.value.green = green;
	def.u.cd.value.blue = blue;
	def.u.cd.value.alpha = 255; //alpha is not actually selectable.
	def.u.cd.dephault = def.u.cd.value;
	const auto err = globalTL_in_data->inter.add_param(globalTL_in_data->effect_ref, -1, &def);
	if (err) throw(err);
	addToLookup(id);
}


/*******************************************************************************************************
Adds an "Angle" parameter
Value is in degrees
(Only to be called during the PF_Cmd_PARAMS_SETUP event)
Note: value is a double. It will be converted by this function to AE fixed-point
*******************************************************************************************************/
void ParameterHelper::AddAngle(ParameterID id, const std::string& name, double value) {
	PF_ParamDef	def{};	//Must be zero initialised.
	def.param_type = PF_Param_ANGLE;
#pragma warning(suppress:26485)
	strncpy_s(def.name, name.c_str(), sizeof(def.name)); //AE only has 32 bytes for a string.  Safe copy prevents overrun.
	def.name[sizeof(def.name) - 1] = 0; //Ensure string is null terminated
	def.uu.id = static_cast<long>(id);
	def.flags = 0;
	def.ui_flags = 0;
	def.u.ad.value = static_cast<PF_Fixed>(65536 * value);  //convert to adobe fixed point
	def.u.ad.dephault = def.u.ad.value;
	const auto err = globalTL_in_data->inter.add_param(globalTL_in_data->effect_ref, -1, &def);
	if (err) throw(err);
	addToLookup(id);
}

/*******************************************************************************************************
Adds an "Layer" parameter
(Only to be called during the PF_Cmd_PARAMS_SETUP event)
*******************************************************************************************************/
void ParameterHelper::AddLayer(ParameterID id, const std::string& name) {
	PF_ParamDef	def{};	//Must be zero initialised.
	def.param_type = PF_Param_LAYER;
#pragma warning(suppress:26485)
	strncpy_s(def.name, name.c_str(), sizeof(def.name)); //AE only has 32 bytes for a string.  Safe copy prevents overrun.
	def.name[sizeof(def.name) - 1] = 0; //Ensure string is null terminated
	def.uu.id = static_cast<long>(id);
	def.flags = 0;
	def.ui_flags = 0;
	const auto err = globalTL_in_data->inter.add_param(globalTL_in_data->effect_ref, -1, &def);
	if (err) throw(err);
	addToLookup(id);
}

/*******************************************************************************************************
Reads the value of a float slider parameter.
*******************************************************************************************************/
double ParameterHelper::ReadSlider(ParameterID paramID) {
	const auto parameterLocation = GetParameterLocation(paramID);
	PF_ParamDef param{};
	globalTL_in_data->inter.checkout_param(globalTL_in_data->effect_ref, parameterLocation, globalTL_in_data->current_time, globalTL_in_data->time_step, globalTL_in_data->time_scale, &param);
	if (param.param_type != PF_Param_FLOAT_SLIDER) throw(std::exception("Parameter is not a slider"));
	const double value = param.u.fs_d.value;
	globalTL_in_data->inter.checkin_param(globalTL_in_data->effect_ref, &param);
	return value;
}

/*******************************************************************************************************
Reads the value an angle paramter (in degrees, may be a multiple of 360).
Note: Internally AE uses fixed-point for this parameter, this function converts it to a double.
*******************************************************************************************************/
double ParameterHelper::ReadAngle(ParameterID paramID) {
	const auto parameterLocation = GetParameterLocation(paramID);
	PF_ParamDef param{};
	globalTL_in_data->inter.checkout_param(globalTL_in_data->effect_ref, parameterLocation, globalTL_in_data->current_time, globalTL_in_data->time_step, globalTL_in_data->time_scale, &param);
	if (param.param_type != PF_Param_ANGLE) throw(std::exception("Parameter is not an angle"));
	const long valueLong = param.u.ad.value;  //this is an AE fixed point value
	globalTL_in_data->inter.checkin_param(globalTL_in_data->effect_ref, &param);
	return static_cast<double>(valueLong) / 65536.0;
}

/*******************************************************************************************************
Reads the value a drop-down paramter
Note: First result is 1.
*******************************************************************************************************/
long ParameterHelper::ReadList(ParameterID paramID) {
	const auto parameterLocation = GetParameterLocation(paramID);
	PF_ParamDef param{};
	globalTL_in_data->inter.checkout_param(globalTL_in_data->effect_ref, parameterLocation, globalTL_in_data->current_time, globalTL_in_data->time_step, globalTL_in_data->time_scale, &param);
	if (param.param_type != PF_Param_POPUP) throw(std::exception("Parameter is not a list"));
	const long value = param.u.pd.value;
	globalTL_in_data->inter.checkin_param(globalTL_in_data->effect_ref, &param);
	return value;
}

/*******************************************************************************************************
Reads the value of a check box parameter.
Uses check-in/check-out, so this is ok to use during a smart-render call.
*******************************************************************************************************/
bool ParameterHelper::ReadCheckBox(ParameterID paramID) {
	const auto parameterLocation = GetParameterLocation(paramID);
	PF_ParamDef param{};
	globalTL_in_data->inter.checkout_param(globalTL_in_data->effect_ref, parameterLocation, globalTL_in_data->current_time, globalTL_in_data->time_step, globalTL_in_data->time_scale, &param);
	if (param.param_type != PF_Param_CHECKBOX) throw(std::exception("Parameter is not a checkbox"));
	const long value = param.u.pd.value;
	globalTL_in_data->inter.checkin_param(globalTL_in_data->effect_ref, &param);
	return value != 0;
}

/*******************************************************************************************************
Reads a colour picker parameter.
Always returns an 8-bit colour value.  User can't select alpha, so we set it to solid.
*******************************************************************************************************/
PF_Pixel ParameterHelper::ReadColour(ParameterID paramID) {
	const auto parameterLocation = GetParameterLocation(paramID);
	PF_ParamDef param{};
	globalTL_in_data->inter.checkout_param(globalTL_in_data->effect_ref, parameterLocation, globalTL_in_data->current_time, globalTL_in_data->time_step, globalTL_in_data->time_scale, &param);
	if (param.param_type != PF_Param_COLOR) throw(std::exception("Parameter is not a color"));
	PF_Pixel color = param.u.cd.value;
	color.alpha = 0xff;
	globalTL_in_data->inter.checkin_param(globalTL_in_data->effect_ref, &param);
	return color;
}

/*******************************************************************************************************
Reads the location (parameter number) of a layer
Note: When using smart rendering you only need the parameter number to check-out a layer. So this function
is just a stub for GetParameterLocation(). (the result of that checkout will nullptr if no layer selected)
*******************************************************************************************************/
inline long ParameterHelper::ReadLayerLocation(ParameterID paramID) {
#ifdef _DEBUG 
	//If debugging, check that it really is a layer parameter.
	const auto parameterLocation = GetParameterLocation(paramID);
	PF_ParamDef param{};
	globalTL_in_data->inter.checkout_param(globalTL_in_data->effect_ref, parameterLocation, globalTL_in_data->current_time, globalTL_in_data->time_step, globalTL_in_data->time_scale, &param);
	if (param.param_type != PF_Param_LAYER) throw(std::exception("Parameter is not a layer"));
	globalTL_in_data->inter.checkin_param(globalTL_in_data->effect_ref, &param);

#endif
	return GetParameterLocation(paramID);
}




/*******************************************************************************************************
Given an id, get the location of a parameter (location is used to index parameters by AE).
*******************************************************************************************************/
inline int ParameterHelper::GetParameterLocation(ParameterID id)
{
	const int idValue = static_cast<int>(id);
	if (idValue <= 0 || idValue >= static_cast<int>(ParameterID::__last)) throw (std::exception("id out of range in GetParameterLocation()"));
	const auto location = paramTranslate.at(idValue);
	if (location <= 0) throw (std::exception("Invalid location in GetParameterLocation(). ParameterID not used."));
	return location;
}









/*******************************************************************************************************
Enable a parameter.
Only call during PF_Cmd_USER_CHANGED_PARAM event.
*******************************************************************************************************/
#pragma warning(suppress:26485)
void ParameterHelper::EnableParameter(ParameterID id, const PF_ParamDef* const  params[])
{
	if (!params) throw (std::exception("Null parameter"));
	const auto loc = GetParameterLocation(id);

#pragma warning(suppress:26481 26489)
	PF_ParamDef def = *params[loc];
	def.ui_flags &= ~PF_PUI_DISABLED;

	AEGP_SuiteHandler suites(globalTL_in_data->pica_basicP);
	const auto err = suites.ParamUtilsSuite3()->PF_UpdateParamUI(globalTL_in_data->effect_ref, loc, &def);
	if (err) throw (err);
}

/*******************************************************************************************************
Disable a parameter.
Only call during PF_Cmd_USER_CHANGED_PARAM event.
*******************************************************************************************************/
#pragma warning(suppress:26485)
void ParameterHelper::DisableParameter(ParameterID id, const PF_ParamDef* const params[])
{
	if (!params) throw (std::exception("Null parameter"));
	const auto loc = GetParameterLocation(id);

#pragma warning(suppress:26481 26489)
	PF_ParamDef def = *params[loc];
	def.ui_flags |= PF_PUI_DISABLED;

	AEGP_SuiteHandler suites(globalTL_in_data->pica_basicP);
	const auto err = suites.ParamUtilsSuite3()->PF_UpdateParamUI(globalTL_in_data->effect_ref, loc, &def);
	if (err) throw (err);
}



/*******************************************************************************************************
Add to lookup.
*******************************************************************************************************/
inline void ParameterHelper::addToLookup(ParameterID id) noexcept {
	paramTranslate.at(static_cast<long>(id)) = paramsAdded;
	paramsAdded++;

}

/*******************************************************************************************************
Private static data members
*******************************************************************************************************/
int ParameterHelper::paramsAdded{ 1 };
std::array<int, static_cast<int>(ParameterID::__last)> ParameterHelper::paramTranslate{};



