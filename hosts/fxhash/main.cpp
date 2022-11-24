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
    Main Entry point. Runs on GUI thread.
    (EMSCRIPTEN only)     
    Host actually consists of 3 seperate wasm programs which run on different threads. (GUI, Background, Workers)
    No shared memory objects are used on the fx(hash) host. Everything is managed by messages. 
*******************************************************************************************************/
#include "jsutil.h"
#include "ui.h"

#include <emscripten.h>

/**************************************************************************************************
 * [Inline Javascript]
 * Checks if the browser supports the required function.  Displays an error message if not.
 * Returns true/false to indicate support
 * ************************************************************************************************/
EM_JS (bool, check_browser_support, (), {
    if (window.Worker && typeof OffscreenCanvas !== 'undefined') return true;
    document.body.innerHTML = "<div style='margin-top:50px;width:100%;text-align:center;'><h3>Unsupported Browser</h3><br><br>The features required by this application are not all supported by your browser.  (WebWorkers, OffscreenCanvas).<br> The minimum supported browser versions are listed below<br><br>Chrome 69<br>Firefox 105<br>Edge 79<br>Opera 56<br><br></div>";
    return false;
});

/**************************************************************************************************
 * [Inline Javascript]
 * ************************************************************************************************/
EM_JS (bool, setLoaded, (), {
    eval("cppLoaded=true;");
});


/**************************************************************************************************
 * Main Entry Point
 * ************************************************************************************************/
int main(){    
    setLoaded();
    if (!check_browser_support()) return 0;
    start_ui();
    return 0;
}















