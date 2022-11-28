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

    The main entry for point for OpenFX Plug-ins
    This is a .dll (renamed .ofx)
********************************************************************************************************/
********************************************************************************************************

Description:

	The main entry for point for OpenFX Plug-ins
	This is a .dll (renamed .ofx)
********************************************************************************************************/

#include "config.h"
#include "openfx-helper.h"
#include "../../common/util.h"


#include <exception>
#include <string.h>

/*******************************************************************************************************
Globals
*******************************************************************************************************/
OfxHost* global_OFXHost{ nullptr };
const OfxImageEffectSuiteV1* global_EffectSuite{ nullptr };
const OfxPropertySuiteV1* global_PropertySuite{ nullptr };
HostData global_hostData{};

/*******************************************************************************************************
Forward Declarations
*******************************************************************************************************/
static OfxStatus OFXPluginMain(const char* action, const void* handle, OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs) noexcept;
static void OFXSetHostFunc(OfxHost* hostStruct) noexcept;
static OfxStatus DescribeAction(const OfxImageEffectHandle effect);
static OfxStatus OnLoadAction(void);
static OfxStatus DescribeInContextAction(const OfxImageEffectHandle effect, OfxPropertySetHandle inArgs);

/*******************************************************************************************************
A struct that provides the host with information about this OFX plugin.
*******************************************************************************************************/
static OfxPlugin pluginStruct =
{
      kOfxImageEffectPluginApi,     //API
      1,                            //API version
      PluginIdentifier,             //Plugin Unique Identifier (see Config.h)
      PluginMajorVersion,           //Plugin Major Version (see Config.h)
      PluginMinorVersion,           //Plugin Minor Version (see Config.h)
      OFXSetHostFunc,               //Set callback function.
      OFXPluginMain                 //Set callback function.
};

/*******************************************************************************************************
(Mandated OFX function)
Returns the plugin struct, that gives the host information about this plug-in.
*******************************************************************************************************/
OfxExport OfxPlugin* OfxGetPlugin(int nth)
{
    
    if (nth == 0) return &pluginStruct;
    return 0;
}

/*******************************************************************************************************
(Mandated OFX function)
Returns the number of plug-ins in this file.
*******************************************************************************************************/
OfxExport int OfxGetNumberOfPlugins(void)
{
    return 1;
}

/*******************************************************************************************************
(OFX Callback)
Callback called by the host to provide information about the host.
*******************************************************************************************************/
static void OFXSetHostFunc(OfxHost* hostStruct) noexcept {
    global_OFXHost = hostStruct;
}

/*******************************************************************************************************
(OFX Callback)
Main entry point for calls to the plug-in.
Dispatches to another function based on the requested action.
*******************************************************************************************************/
static OfxStatus OFXPluginMain(const char* action, const void* handle, [[maybe_unused]]  OfxPropertySetHandle inArgs, OfxPropertySetHandle) noexcept {

    try {
        #pragma warning(suppress:26462 26493) 
        const OfxImageEffectHandle effect = (const OfxImageEffectHandle)handle;  //Effect Handle (A blind struct*)

        //if (strcmp(action, kOfxImageEffectActionRender) == 0) return RenderOFX(effect, inArgs);
        if (strcmp(action, kOfxActionCreateInstance) == 0) return kOfxStatOK;
        if (strcmp(action, kOfxActionDestroyInstance) == 0) return kOfxStatOK;
        if (strcmp(action, kOfxActionLoad) == 0) return OnLoadAction();
        if (strcmp(action, kOfxActionDescribe) == 0) return DescribeAction(effect);
        if (strcmp(action, kOfxImageEffectActionDescribeInContext) == 0) return DescribeInContextAction(effect, inArgs);
        return kOfxStatReplyDefault;
    }
    catch (const OfxStatus) {
        dev_log("Exception (OFX Code) ");
        return kOfxStatFailed;
    }
    catch (const std::exception e) {
        dev_log(std::string("Uncaught Exception: ") + e.what());
        return kOfxStatFailed;
    }
    catch (...) {
        return kOfxStatFailed;
    }

}


/*******************************************************************************************************
"onLoad" Action.

Global startup.

We find out what features the host can support here.  We stash this info in "global_hostData".

From: https://openfx.readthedocs.io/en/master/Reference/ofxImageEffectActions.html
"This action is the first action passed to a plug-in after the binary containing the plug-in has been loaded.
It is there to allow a plug-in to create any global data structures it may need and is also when the plug-in should fetch suites from the host.
The handle, inArgs and outArgs arguments to the mainEntry are redundant and should be set to NULL."
*******************************************************************************************************/
static OfxStatus OnLoadAction(void) {
    dev_log("===================================================\nOnLoad Action");
    if (!global_OFXHost) return kOfxStatErrMissingHostFeature;

    //Get Suites
    global_EffectSuite = static_cast<const OfxImageEffectSuiteV1*>(global_OFXHost->fetchSuite(global_OFXHost->host, kOfxImageEffectSuite, 1));
    if (!global_EffectSuite) return kOfxStatErrMissingHostFeature;

    global_PropertySuite = static_cast<const OfxPropertySuiteV1*>(global_OFXHost->fetchSuite(global_OFXHost->host, kOfxPropertySuite, 1));
    if (!global_PropertySuite) return kOfxStatErrMissingHostFeature;

    char* cstr;
    int count;
    int v;

    //Collect Information about the host.
    global_PropertySuite->propGetString(global_OFXHost->host, kOfxPropLabel, 0, &global_hostData.hostLabel);
    dev_log(std::string("Host Application : ") + global_hostData.hostLabel);

    global_PropertySuite->propGetString(global_OFXHost->host, kOfxPropVersionLabel, 0, &global_hostData.hostVersion);
    dev_log(std::string("Host Version : ") + global_hostData.hostVersion);

    global_PropertySuite->propGetInt(global_OFXHost->host, kOfxImageEffectPropSupportsMultiResolution, 0, &v);
    global_hostData.supportsMultiReolution = static_cast<bool>(v);
    dev_log(std::string("Supports Multi Resolution : ") + truefalse(v));

    global_PropertySuite->propGetInt(global_OFXHost->host, kOfxImageEffectPropSupportsTiles, 0, &v);
    global_hostData.supportsTiles = static_cast<bool>(v);
    dev_log(std::string("Supports Tiles : ") + truefalse(v));

    global_PropertySuite->propGetInt(global_OFXHost->host, kOfxImageEffectPropTemporalClipAccess, 0, &v);
    global_hostData.supportsTemporalClipAccess = static_cast<bool>(v);
    dev_log(std::string("Supports Temporal Clip Access : ") + truefalse(v));

    global_PropertySuite->propGetInt(global_OFXHost->host, kOfxImageEffectPropSupportsOverlays, 0, &v);
    global_hostData.supportsOverlays = static_cast<bool>(v);
    dev_log(std::string("Supports Overlays : ") + truefalse(v));

    global_PropertySuite->propGetInt(global_OFXHost->host, kOfxImageEffectPropSupportsMultipleClipPARs, 0, &v);
    global_hostData.supportsMultipleClipAspectRatios = static_cast<bool>(v);
    dev_log(std::string("Supports Multiple Aspect Ratios : ") + truefalse(v));




    //Check Context Support
    global_PropertySuite->propGetDimension(global_OFXHost->host, kOfxImageEffectPropSupportedContexts, &count);
    for (int i = 0; i < count; i++) {
        global_PropertySuite->propGetString(global_OFXHost->host, kOfxImageEffectPropSupportedContexts, i, &cstr);
        dev_log(std::string("Context Supported : ") + cstr);
        if (strcmp(cstr, kOfxImageEffectContextGenerator) == 0) global_hostData.supportsContextGenerator = true;
        if (strcmp(cstr, kOfxImageEffectContextFilter) == 0)  global_hostData.supportsContextFilter = true;
        if (strcmp(cstr, kOfxImageEffectContextTransition) == 0) global_hostData.supportsContextTransition = true;
        if (strcmp(cstr, kOfxImageEffectContextPaint) == 0) global_hostData.supportsContextPaint = true;
        if (strcmp(cstr, kOfxImageEffectContextGeneral) == 0) global_hostData.supportsContextGeneral = true;
        if (strcmp(cstr, kOfxImageEffectContextRetimer) == 0) global_hostData.supportsContextRetimer = true;
    }
    if (!(global_hostData.supportsContextGenerator || global_hostData.supportsContextFilter || global_hostData.supportsContextGeneral)) return kOfxStatErrMissingHostFeature;


    //Check Pixel Mode Support
    global_PropertySuite->propGetDimension(global_OFXHost->host, kOfxImageEffectPropSupportedComponents, &count);
    for (int i = 0; i < count; i++) {
        global_PropertySuite->propGetString(global_OFXHost->host, kOfxImageEffectPropSupportedComponents, i, &cstr);
        dev_log(std::string("Pixel Mode Supported : ") + cstr);
        if (strcmp(cstr, kOfxImageComponentRGBA) == 0) global_hostData.supportsComponentRGBA = true;
        if (strcmp(cstr, kOfxImageComponentRGB) == 0)  global_hostData.supportsComponentRGB = true;
        if (strcmp(cstr, kOfxImageComponentAlpha) == 0) global_hostData.supportsComponentA = true;
    }
    if (!(global_hostData.supportsComponentRGBA || global_hostData.supportsComponentRGB)) return kOfxStatErrMissingHostFeature;

    global_PropertySuite->propGetInt(global_OFXHost->host, kOfxImageEffectPropSupportsMultipleClipDepths, 0, &v);
    global_hostData.supportsMultipleClipDepths = static_cast<bool>(v);
    dev_log(std::string("Supports Multiple Clip Depths : ") + truefalse(v));


    //Check Bit Depth.
    //Not technically a supported call here according to https://openfx.readthedocs.io/en/master/Reference/ofxPropertiesByObject.html#properties-on-the-image-effect-host
    //However, it seems to be supported by hosts, so we will read it if there is data.
    global_PropertySuite->propGetDimension(global_OFXHost->host, kOfxImageEffectPropSupportedPixelDepths, &count);
    if (count > 0) {
        for (int i = 0; i < count; i++) {
            global_PropertySuite->propGetString(global_OFXHost->host, kOfxImageEffectPropSupportedPixelDepths, i, &cstr);
            dev_log(std::string("Pixel Depth Supported : ") + cstr);
            if (strcmp(cstr, kOfxBitDepthByte) == 0) global_hostData.supportsBitDepthByte = true;
            if (strcmp(cstr, kOfxBitDepthShort) == 0)  global_hostData.supportsBitDepthShort = true;
            if (strcmp(cstr, kOfxBitDepthHalf) == 0) global_hostData.supportsBitDepthHalf = true;
            if (strcmp(cstr, kOfxBitDepthFloat) == 0) global_hostData.supportsBitDepthFloat = true;
        }
        if (!(global_hostData.supportsBitDepthByte || global_hostData.supportsBitDepthFloat)) return kOfxStatErrMissingHostFeature;
    }

    return kOfxStatOK;
}




/*******************************************************************************************************
"describe" Action.

It is our turn to tell the host what we can support.

From: https://openfx.readthedocs.io/en/master/Reference/ofxImageEffectActions.html
"The kOfxActionDescribe is the second action passed to a plug-in. It is where a plugin defines how it behaves and the resources it needs to function.
Note that the handle passed in acts as a descriptor for, rather than an instance of the plugin. The handle is global and unique. 
The plug-in is at liberty to cache the handle away for future reference until the plug-in is unloaded.
Most importantly, the effect must set what image effect contexts it is capable of working in.
This action must be trapped, it is not optional."
*******************************************************************************************************/
static OfxStatus DescribeAction(const OfxImageEffectHandle effect) {
    dev_log("Describe Action");
    //Get the effect's properties structure. (This is a blind structure manipulated with the PropertySuite).
    OfxPropertySetHandle effectProperties;
    CheckOFX(global_EffectSuite->getPropertySet(effect, &effectProperties));



    //Set the effects properties.
    CheckOFX(global_PropertySuite->propSetString(effectProperties, kOfxPropLabel, 0, PluginName));                                        //Plug-in name
    CheckOFX(global_PropertySuite->propSetString(effectProperties, kOfxImageEffectPluginPropGrouping, 0, PluginMenu));                    //Plug-in menu location
    CheckOFX(global_PropertySuite->propSetInt(effectProperties, kOfxImageEffectPropSupportsTiles, 0, false));                             //Disable tiling (we want whole frames)


    //Indicate which bit depths we can support.
    //Note: Resolve seems to always send Floats as images, regarless of what we ask for.
    CheckOFX(global_PropertySuite->propSetInt(effectProperties, kOfxImageEffectPropSupportsMultipleClipDepths, 0, false));                //Multiple Bit Depths
    CheckOFX(global_PropertySuite->propSetString(effectProperties, kOfxImageEffectPropSupportedPixelDepths, 0, kOfxBitDepthByte));        //8 Bit Colour
    CheckOFX(global_PropertySuite->propSetString(effectProperties, kOfxImageEffectPropSupportedPixelDepths, 2, kOfxBitDepthFloat));       //32 Bit Float Colour

    // define the contexts we can be used in
    CheckOFX(global_PropertySuite->propSetString(effectProperties, kOfxImageEffectPropSupportedContexts, 0, kOfxImageEffectContextGenerator));  //Support Generator context
    CheckOFX(global_PropertySuite->propSetString(effectProperties, kOfxImageEffectPropSupportedContexts, 1, kOfxImageEffectContextFilter));     //Support Effect Context
    CheckOFX(global_PropertySuite->propSetString(effectProperties, kOfxImageEffectPropSupportedContexts, 2, kOfxImageEffectContextGeneral));    //General Effect Context
    return kOfxStatOK;
}

/*******************************************************************************************************
"describeInContext" Action.

Describe what each specific context can support. (Input & Output format specifications.)
Resolve calls this when effect is applied.

From: https://openfx.readthedocs.io/en/master/Reference/ofxImageEffectActions.html
"This action is unique to OFX Image Effect plug-ins. Because a plugin is able to exhibit different behaviour depending on the context of use,
each separate context will need to be described individually. It is within this action that image effects describe which parameters and input clips it requires.
This action will be called multiple times, one for each of the contexts the plugin says it is capable of implementing. If a host does not support a certain context,
then it need not call kOfxImageEffectActionDescribeInContext for that context."
*******************************************************************************************************/
static OfxStatus DescribeInContextAction(const OfxImageEffectHandle effect, OfxPropertySetHandle inArgs) {
    dev_log("DescribeInContext Action");

    //Find out which context we are setting up.
    char* contextString;
    OFXContext context;
    CheckOFX(global_PropertySuite->propGetString(inArgs, kOfxImageEffectPropContext, 0, &contextString));
    context = GetContextFromString(contextString);
    if (context == OFXContext::invalid) return kOfxStatFailed;
    dev_log(std::string("Context is ") + contextString);


    //A properties structure (blind struct, manipulated with PropertySuite)
    OfxPropertySetHandle properties;

    //Define the mandated output clip for all contexts
    dev_log("Adding Output Clip");
    CheckOFX(global_EffectSuite->clipDefine(effect, "Output", &properties));
    if (global_hostData.supportsComponentRGBA) CheckOFX(global_PropertySuite->propSetString(properties, kOfxImageEffectPropSupportedComponents, 0, kOfxImageComponentRGBA));  //RGBA format 
    if (global_hostData.supportsComponentRGB) CheckOFX(global_PropertySuite->propSetString(properties, kOfxImageEffectPropSupportedComponents, 1, kOfxImageComponentRGB));   //RGB format 



    //We only have an input clip for general and filter contexts.
    if (context == OFXContext::filter || context == OFXContext::general) {
        dev_log("Adding Input Clip");
        CheckOFX(global_EffectSuite->clipDefine(effect, "Source", &properties));
        if (global_hostData.supportsComponentRGBA) CheckOFX(global_PropertySuite->propSetString(properties, kOfxImageEffectPropSupportedComponents, 0, kOfxImageComponentRGBA)); //RGBA format
        if (global_hostData.supportsComponentRGB) CheckOFX(global_PropertySuite->propSetString(properties, kOfxImageEffectPropSupportedComponents, 1, kOfxImageComponentRGB)); //RGB format
    }
    //dev_log("DescribeInContext Action Complete");
    return kOfxStatOK;
}