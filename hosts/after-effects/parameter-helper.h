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
#pragma once
#include "ae.h"
#include "parameter-id.h"
#include <array>
#include <string>

class ParameterHelper {
private:
	static int paramsAdded;  ///Counter used to track parameters as they are added .
	static std::array<int, static_cast<int>(ParameterID::__last)> paramTranslate; ///To translate param ID to location

public:
	static void AddSlider(ParameterID id, const std::string& name, float min, float max, float sliderMin, float sliderMax, float value, short precision);
	static void AddCheckBox(ParameterID id, const std::string& name, const std::string& comment, bool value);
	static void AddButton(ParameterID id, const std::string& name, const std::string& buttonText);
	static void AddList(ParameterID id, const std::string& name, const std::string& choices, short value, bool supervise = false);
	static void AddGroupStart(ParameterID id, const std::string name);
	static void AddGroupEnd(ParameterID id);
	static void AddColourPickerRGB(ParameterID id, const std::string& name, unsigned char red, unsigned char green, unsigned char blue);
	static void AddAngle(ParameterID id, const std::string& name, double value);
	static void AddLayer(ParameterID id, const std::string& name);

	static double ReadSlider(ParameterID paramID);
	static double ReadAngle(ParameterID paramID);
	static long ReadList(ParameterID paramID);
	static bool ReadCheckBox(ParameterID paramID);
	static PF_Pixel ReadColour(ParameterID paramID);
	static long ReadLayerLocation(ParameterID paramID);
	static int GetNumberParameters() noexcept { return paramsAdded; }
	static int GetParameterLocation(ParameterID id);

	static void EnableParameter(ParameterID id, const PF_ParamDef* const params[]);
	static void DisableParameter(ParameterID id, const PF_ParamDef* const params[]);

private:
	static void addToLookup(ParameterID id) noexcept;
};