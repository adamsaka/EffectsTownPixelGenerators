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

   (EMSCRIPTEN only)
   General utility functions for interfacing with Javascript.
*************************************************************************************************/
#include "jsutil.h"

#include <emscripten.h>
#include <emscripten/val.h>

using namespace emscripten;



/**************************************************************************************************

 *************************************************************************************************/
bool js_is_defined(std::string var){
    return emscripten_run_script_int((std::string("typeof(") + var + ") != \"undefined\"").c_str());
}



/**************************************************************************************************

 *************************************************************************************************/
void js_console_log(const std::string &s){
    EM_ASM({
        console.log(UTF8ToString($0));
    },s.c_str());
}






