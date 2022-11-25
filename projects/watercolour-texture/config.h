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

	Project configuration


*******************************************************************************************************/
#pragma once



//**WARNING**: ALSO update these constants in AE.r.  They must match.
#define PluginName					"Watercolour Texture"
#define PluginMenu					"Effects Town"
#define PluginIdentifier			"Town.Effects.WatercolourTexture"
#define	PluginMajorVersion			1
#define	PluginMinorVersion			0
#define	PluginBugVersion			0
#define	PluginBuildVersion			1

//Indicates that a project will not return any transparent pixels.
constexpr bool project_is_solid_render = true;

//Floating point precesion to use for this project.
typedef double Precision;

