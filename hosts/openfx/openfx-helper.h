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

	The main entry for point for OpenFX Plug-ins
	This is a .dll (renamed .ofx)
********************************************************************************************************/
#pragma once

#include "../../common/util.h"

//OpenFX C API Headers
#include "ofxCore.h"
#include "ofxProperty.h"
#include "ofxImageEffect.h"

#include <cstring>
#include <cstdint>
#include <string>



struct HostData;

//OpenFX Globals
extern OfxHost* global_OFXHost;								//Holds the global host information for an OpenFX plugin (in OpenFX-Main.cpp)
extern const OfxImageEffectSuiteV1* global_EffectSuite;
extern const OfxPropertySuiteV1* global_PropertySuite;
extern HostData global_hostData;


/*******************************************************************************************************
Enum to hold context type.
*******************************************************************************************************/
enum class OFXContext {
    invalid,
    generator,
    filter,
    transition,
    paint,
    retimer,
    general
};


/*******************************************************************************************************
Throws code if there is an error
*******************************************************************************************************/
/*inline void CheckOFX(OfxStatus status) {
    if (status != kOfxStatOK) throw(status);
}*/
#define CheckOFX(STATUS) if((STATUS != kOfxStatOK )) {dev_log(std::string("OFX Error : ") + std::string(__FILE__) + " " +std::to_string(__LINE__)) ; throw(STATUS); }


/*******************************************************************************************************
Get the context type as an enum from a string.
*******************************************************************************************************/
inline OFXContext GetContextFromString(const char* contextString) noexcept {
    if (strcmp(contextString, kOfxImageEffectContextFilter) == 0) return OFXContext::filter;
    if (strcmp(contextString, kOfxImageEffectContextGeneral) == 0) return OFXContext::general;
    if (strcmp(contextString, kOfxImageEffectContextGenerator) == 0) return OFXContext::generator;
    if (strcmp(contextString, kOfxImageEffectContextTransition) == 0) return OFXContext::transition;
    if (strcmp(contextString, kOfxImageEffectContextPaint) == 0) return OFXContext::paint;
    if (strcmp(contextString, kOfxImageEffectContextRetimer) == 0) return OFXContext::retimer;
    return OFXContext::invalid;
}

/*******************************************************************************************************
Get the number of components per pixel from a string. (4=RGBA, 3-RGB, 1=Alpha, 0=None/Unknown);
*******************************************************************************************************/
inline int GetCompentsPerPixelFromString(const char* string) noexcept {
    if (strcmp(string, kOfxImageComponentRGBA) == 0) return 4;
    if (strcmp(string, kOfxImageComponentRGB) == 0) return 3;
    if (strcmp(string, kOfxImageComponentAlpha) == 0) return 1;
    return 0;
}

/*******************************************************************************************************
Gets the bit depth from a string.  (Half is ignored, and returned as zero).
*******************************************************************************************************/
inline int GetBitDepthFromString(const char* string) noexcept {
    if (strcmp(string, kOfxBitDepthByte) == 0) return 8;
    if (strcmp(string, kOfxBitDepthShort) == 0) return 16;
    if (strcmp(string, kOfxBitDepthFloat) == 0) return 32;
    return 0;
}

/*******************************************************************************************************
Store data on the host, that we readout in the OnLoad event.
*******************************************************************************************************/
struct HostData {
    char* hostLabel{ nullptr };
    char* hostVersion{ nullptr };
    bool supportsMultiReolution{ false };
    bool supportsTiles{ false };
    bool supportsTemporalClipAccess{ false };
    bool supportsComponentRGB{ false };
    bool supportsComponentRGBA{ false };
    bool supportsComponentA{ false };
    bool supportsContextGenerator{ false };
    bool supportsContextFilter{ false };
    bool supportsContextTransition{ false };
    bool supportsContextPaint{ false };
    bool supportsContextGeneral{ false };
    bool supportsContextRetimer{ false };
    bool supportsBitDepthByte{ false };
    bool supportsBitDepthShort{ false };
    bool supportsBitDepthFloat{ false };
    bool supportsBitDepthHalf{ false };
    bool supportsOverlays{ false };
    bool supportsMultipleClipDepths{ false };
    bool supportsMultipleClipAspectRatios{ false };



};



/*******************************************************************************************************
RAII struct to aquire and release OFX image handles & images.
*******************************************************************************************************/
struct ClipHolder {
    OfxImageClipHandle clipHandle{ nullptr };
    OfxPropertySetHandle clipImage{ nullptr };
    long rowBytes;               //offset in bytes of each row (may be negative)
    OfxRectI bounds;            //Bounding Rectangle
    uint8_t* baseAddress;          //Base Address of data
    size_t componentsPerPixel;     //Pixel Format (4=RGBA, 3=RGB, 1=Alpha, 0=None/Unknown);
    int bitDepth;                   //8=Byte, 16=Short 32=Float
    bool preMultiplied{ true };

    /*******************************************************************************************************
    Contructor.  Will throw an OFXStatus (int) if we fail to initialise.
    *******************************************************************************************************/
    ClipHolder(const OfxImageEffectHandle instance, const char* clipName, OfxTime time) {
        char* cstr{};
        int rowBytes32{};
        dev_log(std::string("Load Clip: ") + clipName);
        //Get Clip Handle and property set
        OfxPropertySetHandle clipInstanceProperties;
        CheckOFX(global_EffectSuite->clipGetHandle(instance, clipName, &clipHandle, &clipInstanceProperties));
        if (!clipHandle) throw (kOfxStatFailed);


        //Get Bit Depth from image descriptor
        CheckOFX(global_PropertySuite->propGetString(clipInstanceProperties, kOfxImageEffectPropPixelDepth, 0, &cstr));
        bitDepth = GetBitDepthFromString(cstr);
        dev_log(cstr);
        if (bitDepth == 0) {
            dev_log(std::string("ERROR: Unsupported Bit Depth: ") + cstr);
            if (strcmp(global_hostData.hostLabel, "DaVinci Resolve") == 0) {
                dev_log("Host is DaVinci Resolve, so ignoring for now.  We will check it in the instance. (Due to bug in generator context v17.1.1)");
            }
            else {
                throw (kOfxStatFailed);
            }
        }

        //Get clip pixel layout from image descriptor
        CheckOFX(global_PropertySuite->propGetString(clipInstanceProperties, kOfxImageEffectPropComponents, 0, &cstr));
        componentsPerPixel = GetCompentsPerPixelFromString(cstr);
        dev_log(cstr);
        if (!(componentsPerPixel == 4 || componentsPerPixel == 3 || componentsPerPixel == 1)) {
            dev_log(std::string("ERROR: Unsupported Pixel Format: ") + cstr);
            if (componentsPerPixel == 0 && strcmp(global_hostData.hostLabel, "DaVinci Resolve") == 0) {
                dev_log("Host is DaVinci Resolve, so assuming bit depth is RGBA.  (Due to bug in generator context v17.1.1)");
                componentsPerPixel = 4;
            }
            else {

                throw (kOfxStatErrUnsupported);
            }
        }

        //Get Pre Multiplication
        CheckOFX(global_PropertySuite->propGetString(clipInstanceProperties, kOfxImageEffectPropPreMultiplication, 0, &cstr));
        if (strcmp(cstr, kOfxImageUnPreMultiplied) == 0)  preMultiplied = false;
        dev_log(cstr);

        //Get clip image
        CheckOFX(global_EffectSuite->clipGetImage(clipHandle, time, NULL, &clipImage));
        if (!clipImage) throw (kOfxStatFailed);

        try {
            //Get clip rectangle
            CheckOFX(global_PropertySuite->propGetIntN(clipImage, kOfxImagePropBounds, 4, &bounds.x1));

            //Get clip pointer and row addressing mode
#pragma warning(suppress:26490) 
            CheckOFX(global_PropertySuite->propGetPointer(clipImage, kOfxImagePropData, 0, reinterpret_cast<void**>(&baseAddress)));
            if (!baseAddress) throw (kOfxStatFailed);
            CheckOFX(global_PropertySuite->propGetInt(clipImage, kOfxImagePropRowBytes, 0, &rowBytes32));
            if (rowBytes32 == 0) throw (kOfxStatFailed);
            rowBytes = rowBytes32;

            //Check bit depth of image matches descriptor (some hosts report 0, so we take the higher option).
            CheckOFX(global_PropertySuite->propGetString(clipImage, kOfxImageEffectPropPixelDepth, 0, &cstr));
            if (bitDepth != GetBitDepthFromString(cstr)) {
                dev_log(std::string("Mismatch between bit depths (descripter vs instance): ") + cstr);
                bitDepth = std::max(bitDepth, GetBitDepthFromString(cstr));
            }

            //Get clip pixel layout matches the descriptor found above (some hosts report 0, so we take the higher option)
            CheckOFX(global_PropertySuite->propGetString(clipImage, kOfxImageEffectPropComponents, 0, &cstr));
            if (componentsPerPixel != GetCompentsPerPixelFromString(cstr)) {
                dev_log(std::string("Mismatch between components (descripter vs instance): ") + cstr);
                componentsPerPixel = std::max(componentsPerPixel, static_cast<size_t>(GetCompentsPerPixelFromString(cstr)));

            }
            dev_log(std::to_string(bitDepth));
            dev_log(std::to_string(componentsPerPixel));


        }
        catch (OfxStatus status) {
            //Need to release the image if we fail initialisation after aquiring it
            global_EffectSuite->clipReleaseImage(clipImage);
            throw(status);
        }
    }

    /*******************************************************************************************************
    Destructor: Release the clip back to the host.
    *******************************************************************************************************/
    ~ClipHolder() {
        if (clipImage) {
            global_EffectSuite->clipReleaseImage(clipImage);
        }
    }

    //Delete copy operations
    ClipHolder(const ClipHolder& s) = delete;
    ClipHolder(ClipHolder&& s) = delete;
    ClipHolder& operator=(const ClipHolder& s) = delete;
    ClipHolder& operator=(ClipHolder&& s) = delete;

#pragma warning(push)
#pragma warning(disable:26490 26481 26472) 

    /*******************************************************************************************************
    Get a pointer to the start of the row.  Will return nullptr if out of bounds.
    *******************************************************************************************************/
    float* rowAddressFloat(long y) {
        if (y < bounds.y1 || y >= bounds.y2) return nullptr;
        return reinterpret_cast<float*>(baseAddress + (y - static_cast<long>(bounds.y1)) * rowBytes);
    }

    /*******************************************************************************************************
    Get a pointer to the start of the row.  Will return nullptr if out of bounds.
    *******************************************************************************************************/
    uint16_t* rowAddress16(long y) noexcept {
        if (y < bounds.y1 || y >= bounds.y2) return nullptr;
        return reinterpret_cast<uint16_t*>(baseAddress + (y - static_cast<long>(bounds.y1)) * rowBytes);
    }

    /*******************************************************************************************************
    Get a pointer to the start of the row.  Will return nullptr if out of bounds.
    *******************************************************************************************************/
    uint8_t* rowAddress8(long y) noexcept {
        if (y < bounds.y1 || y >= bounds.y2) return nullptr;
        return reinterpret_cast<uint8_t*>(baseAddress + (y - static_cast<long>(bounds.y1)) * rowBytes);
    }
    /*******************************************************************************************************
    Get a pointer to a specific pixel.  Will return nullptr if out of bounds.
    *******************************************************************************************************/
    float* pixelAddressFloat(long x, long y) noexcept {
        if (x < bounds.x1 || x >= bounds.x2 || y < bounds.y1 || y >= bounds.y2) return nullptr;
        auto row = rowAddressFloat(y);
        return row + (x - static_cast<long>(bounds.x1)) * componentsPerPixel;
    }


    /*******************************************************************************************************
    Get a pointer to a specific pixel.  Will return nullptr if out of bounds.
    *******************************************************************************************************/
    uint16_t* pixelAddress16(long x, long y) noexcept {
        if (x < bounds.x1 || x >= bounds.x2 || y < bounds.y1 || y >= bounds.y2) return nullptr;
        auto row = rowAddress16(y);
        return row + (x - static_cast<long>(bounds.x1)) * componentsPerPixel;
    }

    /*******************************************************************************************************
    Get a pointer to a specific pixel.  Will return nullptr if out of bounds.
    *******************************************************************************************************/
    uint8_t* pixelAddress8(long x, long y) noexcept {
        if (x < bounds.x1 || x >= bounds.x2 || y < bounds.y1 || y >= bounds.y2) return nullptr;
        auto row = rowAddress8(y);
        return row + (x - static_cast<long>(bounds.x1)) * componentsPerPixel;
    }
#pragma warning(pop)
};