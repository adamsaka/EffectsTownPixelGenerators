/*********************************************************************************************************
This program is free software: you can redistribute it and/or modify it under the terms of the 
GNU General Public License as published by the Free Software Foundation, either version 3 of the License, 
or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. 
If not, see <https://www.gnu.org/licenses/>. 

Authors:
	Adam - Maths Town

*********************************************************************************************************/

/**************************************************************************************************
 * (EMSCRIPTEN only)
 * main-background.cpp
 * 
 * Called as a WebWorker
 * Main Background Worker Thread Entry Point.  This thread manages rendering.
 * Owns the OffscreenCanvas.
 * This threads starts a number of child "Render Workers" for the actual calculations.
 * ************************************************************************************************/

#include "jsutil.h"

#include <emscripten.h>
#include <emscripten/val.h>


/**************************************************************************************************
 * 
 * [Inline Javascript]
 * 
 * ************************************************************************************************/
EM_JS (emscripten::EM_VAL, setup_workers, (), {
    let offscreen;
    let ctx;
    let backBuffer;

    let isPreview=false;
    let seed = "";

    let workers = [];
    let workerCount = 0;

    let jobNumber = 0;        /// Current Job (update on resize etc).  Zero indicates not ready.
    let lineNumber = 0;       /// Next line to send to worker for rendering
    let linesRendered=0;      /// Number of lines completed by workers
    let renderStartTime = 0;  /// Timing variable
    let lastBufferSwap=0;     /// Timer variable used to rate limit updating of canvas.

    let numberWorkers = navigator.hardwareConcurrency;
    if (numberWorkers<4) numberWorkers=4;
    if (numberWorkers>64) numberWorkers=64;  //We start getting memory allocation issues above 80 or so.

    //Setup message handeling.
    self.onmessage = handelMessageParent;

    //Start Render Workers
    for (let i=0; i<numberWorkers; i++){
        let worker = new Worker("main-render-worker-cpp.js", {name:'render'});
        worker.onmessage = handelMessageRender;       
    }

    //Send loaded message to UI Thread.
    postMessage({'loaded':true});  //Note: Using string indexes to avoid minification by closure compiler.

    /**************************************************************************************************
    * Javascript function:  
    * ************************************************************************************************/
    function startWorkerRender(w){
        
        if (lineNumber >= offscreen.height) return;
        
        //Note: Using string indexes to avoid minification by closure compiler.
        let msg = {
            'render':true,
            'jobNumber': jobNumber,
            'width': offscreen.width,
            'height': offscreen.height,
            'line':lineNumber++,
            'seed':seed,
            'isPreview':isPreview,
        };
        w.postMessage(msg);
        
    }

    /**************************************************************************************************
    * Javascript function:  Perform Render
    * ************************************************************************************************/
    function doRender(){
        //At least one worker needs to be started.        
        if (workerCount == 0) {
           setTimeout(doRender, 20);
           return;
        }
        
        renderStartTime = performance.now();
        lastBufferSwap = performance.now();
        jobNumber++;
        lineNumber=0;
        linesRendered=0;
        

        for (let i=0; i< workerCount; i++){
            startWorkerRender(workers[i]);
        }

        //Clear backbuffer
        for (let i=0;i<backBuffer.data.length;i++) backBuffer.data[i] = 0x0; 
    }

    /**************************************************************************************************
    * Javascript function:  Resize event.
    * A resize event is sent via message from the GUI thread, but we need to resize canvas here.
    * ************************************************************************************************/
    function resizeCanvas(width,height){
        ctx.width = offscreen.width = width;
        ctx.height = offscreen.height = height;
        backBuffer = new ImageData(offscreen.width,offscreen.height);
        doRender();
    }

    /**************************************************************************************************
    * Javascript function: Handels messages from GUI thread (Parent thread)
    * ************************************************************************************************/
    function handelMessageParent(msg){                
        // We will be sent an offscreen canvas
        if(msg.data.hasOwnProperty('canvas')){  

            offscreen = msg.data.canvas;
            ctx = offscreen.getContext('2d');
            backBuffer = new ImageData(offscreen.width,offscreen.height);
            
            doRender();
            
            return;
        }

        if(msg.data.hasOwnProperty('seed')){
            seed = msg.data['seed'];
            isPreview = msg.data['isPreview'];
            return;
        }

        // Don't process other messages if we don't have the canvas.
        if (typeof(offscreen) == "undefined" || typeof(ctx) == "undefined") return;

        //Canvas needs to be resized to match page.
        if (msg.data.hasOwnProperty('resize')){
            resizeCanvas(msg.data.width, msg.data.height);
            return;
        }
        

    }

    /**************************************************************************************************
    * Javascript function:
    * ************************************************************************************************/
    function onRenderComplete(){
        const timeTaken = performance.now() - renderStartTime;
        console.log("Render Complete: " + timeTaken.toFixed(1) + " ms (" + offscreen.width + " x " + offscreen.height  +" pixels)");
    }

    /**************************************************************************************************
    * Javascript function:
    * ************************************************************************************************/
    function processRenderedLine(buf, line){
        linesRendered++;
        const u8view = new Uint8Array(buf);        
        const offset = backBuffer.width*4*line; 
        
        backBuffer.data.set(u8view,offset);

        //update offscreen canvas if a small amount of time has passed since last update or last couple of lines
        let now = performance.now();
        if ((linesRendered == offscreen.height) || ( now - lastBufferSwap > 200)){
            ctx.putImageData(backBuffer,0,0);
            lastBufferSwap = performance.now();   
        }

        if(linesRendered == offscreen.height) onRenderComplete();
    }

    /**************************************************************************************************
    * Javascript function: Handels messages from render workers (children of this thead)
    * ************************************************************************************************/
    function handelMessageRender(msg){   
        let w = msg.target;
        if (msg.data.hasOwnProperty('result')){            
            startWorkerRender(w);
            if (msg.data['jobNumber'] == jobNumber) processRenderedLine(msg.data['buffer'], msg.data['line']);
            return;
        }

        if(msg.data.hasOwnProperty('loaded')){  
            if (jobNumber>0) startWorkerRender(w); //If we have already started rendering we should initiate this thread              
            workers.push(w);
            workerCount++;
            if (workerCount == numberWorkers) console.log(workerCount + " workers started.");
            return;
        }
        
    }
   
});




/**************************************************************************************************
 * Main Entry Point for WebWorker
 * ************************************************************************************************/
int main(){     
    //js_console_log("Background Thread Started");
    setup_workers();

    return 0;
}


