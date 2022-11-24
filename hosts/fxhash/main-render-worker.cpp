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
 * main-render-worker.cpp 
 * 
 * 
 * Called as a WebWorker
 * ************************************************************************************************/


#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

#include "../../projects/watercolour-texture/renderer/renderer.h"
#include "../../common/colour.h"
#include "jsutil.h"

thread_local Renderer<double> renderer {};

/**************************************************************************************************
 * [Inline Javascript]
 * 
 * ************************************************************************************************/
EM_JS (bool, run_javascipt, (), {
    //console.log("Render Worker Loaded");

    function render_line(data){
        var buf = new ArrayBuffer(data.width*4);
        var u32 = new Uint32Array(buf);        
        
        Module["setup_renderer"](data['width'], data['height']);
        Module["set_seed"](data['seed']);
        //console.log(data['seed']);
        for (let x=0; x< data['width']; x++){
            u32[x] = Module["render_pixel"](x, data['line']);
        }        
    
        postMessage({'result':true, 'buffer':buf, 'jobNumber': data['jobNumber'], 'line':data['line']},[buf]);
    }
    
    
    function handelMessage(msg){ 
        if(msg.data.hasOwnProperty('render')){  
            //console.log("Render Request Line : ", msg.data.line);
            render_line(msg.data);
            return;
        }        
    }

    self.onmessage = handelMessage;

    //Send loaded message to parent thread.
    postMessage({'loaded':true}); //Note: Using string indexes to avoid minification by closure compiler.
    return true;
})

/**************************************************************************************************

*************************************************************************************************/
void setup_renderer(uint32_t width, uint32_t height){
    renderer.set_size(width,height);
}

/**************************************************************************************************

*************************************************************************************************/
void set_seed(const std::string str){
    renderer.set_seed(str);
}

/**************************************************************************************************
 * Render Pixel
 * Exported to JS
 * ************************************************************************************************/
uint32_t render_pixel(uint32_t x, uint32_t y){
    if (renderer.get_width() <= 0 || renderer.get_height() <=0) return 0xff0000ff; //Return red if caller has not set size.
    
    /*double xf = static_cast<double>(x) / static_cast<double>(renderer.get_width());
    double yf = static_cast<double>(y) / static_cast<double>(renderer.get_height());

    const auto a = mix_colours(ColourSRGB(1.0, 1.0, 0.0),ColourSRGB(1.0, 0.0, 1.0), xf);
    const auto b = mix_colours(ColourSRGB(0.0, 1.0, 1.0),ColourSRGB(1.0, 0.5, 1.0), xf);
    const auto c = mix_colours(a,b,yf);
    */

    auto c = renderer.render_pixel(x,y);

    return c.to_colour8().to_uint32_keep_memory_layout();

}


/**************************************************************************************************
 * Main Entry Point for WebWorker
 * ************************************************************************************************/
int main(){     
    
    
    run_javascipt();

   

    return 0;
}
using namespace emscripten;
EMSCRIPTEN_BINDINGS(main_render_worker){
    function("render_pixel", &render_pixel);
    function("setup_renderer", &setup_renderer); 
    function("set_seed", &set_seed); 
}
