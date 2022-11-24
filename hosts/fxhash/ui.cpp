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
    Manages user iterface, and iterfacing with javascript.
    (EMSCRIPTEN only)
    Runs on main GUI thread.
********************************************************************************************************/
#include "jsutil.h"

#include <emscripten.h>
#include <emscripten/bind.h>


using namespace emscripten;

emscripten::val main_worker {};


/*************************************Worker Mangagement*******************************************/

/**************************************************************************************************
 * [Inline Javascript]
 * javascript_run_ui
 *  
 * Javascript for the UI.  
 * A background worker is started and control of rendering is handed over to it.
 * Browser compatability checks should already be done.
 * ************************************************************************************************/
EM_JS (bool, javascript_run_ui, (), {
    let seed = eval("fxhash"); //wrapped in eval to prevent minification
    let isPreview = eval("isFxpreview");  
    console.log("Seed: " + seed);
    
    //Start background worker.  We will eventually hand over the canvas to the background worker.
    let worker;
    if (window.Worker){
        worker = new Worker("main-background-cpp.js", {name:'background'});
        worker.onmessage = handelMessage;
        
    }else{
        alert("Web Workers are not supported."); //Should be unreachable, as we checked earlier
    }

    //Setup Events
    window.addEventListener('resize',onResize);    
    
    /**************************************************************************************************
    * Javascript function:
    * ************************************************************************************************/
    function onResize(){
            let canvas =  document.getElementById('canvas');
            worker.postMessage({resize:true, width: canvas.clientWidth, height: canvas.clientHeight,});
    }

    /**************************************************************************************************
    * Javascript function:
    * ************************************************************************************************/
    function sendCanvasToWorker(worker){
        let canvas = document.getElementById("canvas");
        canvas.width = canvas.clientWidth;
        canvas.height = canvas.clientHeight;
        let offscreen = canvas.transferControlToOffscreen();
        worker.postMessage({canvas: offscreen},[offscreen]);
    }
    
    /**************************************************************************************************
    * Javascript function:
    * ************************************************************************************************/
    function handelMessage(msg){                
        if(msg.data.hasOwnProperty('loaded')){  //Initial Worker Loaded Message
            worker.postMessage({'seed':seed, 'isPreview':isPreview});
            sendCanvasToWorker(worker);
            Module["on_worker_load"]();   //callback to c++         
            return;
        }
        Module["on_worker_message"](msg);
    }
    
    return true;
    
})




/**************************************************************************************************
Starts a worker with script file "workerFile"
Returns a val of the started worker object.
*************************************************************************************************/
bool start_ui(){
    return javascript_run_ui();
}

/**************************************************************************************************
[Exported]
The worker thread has completed loading.
*************************************************************************************************/
void on_worker_loaded(){
    //js_console_log("Main Worker Ready");
}

/**************************************************************************************************
[Exported]
The worker has sent a message.
*************************************************************************************************/
void on_worker_message(__attribute__((unused)) val v){
    js_console_log("Worker Message Received");
}

//Bindings to be exported to javascript.
//Note: Closure compile used, so access binding with Module[""];
EMSCRIPTEN_BINDINGS(ui){
    function("on_worker_load", &on_worker_loaded);  
    function("on_worker_message", &on_worker_message);
}

