/*
 * Copyright (c) 2016-2018, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ArgusHelpers.h"
#include "CommonOptions.h"
#include "Error.h"
#include "EGLGlobal.h"
#include "GLContext.h"
#include "JPEGConsumer.h"
#include "PreviewConsumer.h"
#include "Window.h"
#include "Thread.h"

#include <Argus/Argus.h>

#include <unistd.h>
#include <stdlib.h>
#include <sstream>
#include <iomanip>

#include <chrono>

using namespace Argus;
using namespace EGLStream;

int main(int argc, char** argv)
{
    
    // Initialize the Argus camera provider. 
    UniqueObj<CameraProvider> g_cameraProvider; 
    g_cameraProvider = UniqueObj<CameraProvider> (CameraProvider::create());

    // Get the ICameraProvider interface from the global CameraProvider 
    ICameraProvider *iCameraProvider = interface_cast<ICameraProvider>(g_cameraProvider); 
    if (!iCameraProvider)
    {
        ORIGINATE_ERROR("Failed to get ICameraProvider interface");
    } 


    // Get the camera devices. 
    std::vector<CameraDevice*> g_cameraDevices; 
    iCameraProvider->getCameraDevices(&g_cameraDevices); 
    if (g_cameraDevices.size() == 0) 
    { 
        ORIGINATE_ERROR("there are %d cameras",(unsigned)g_cameraDevices.size()); 
    }


    std::vector<SensorMode*> sensormodes; 
    ICameraProperties *iCameraProperties = interface_cast<ICameraProperties>(g_cameraDevices[0]); 
    if (!iCameraProperties){
        ORIGINATE_ERROR("Failed to get ICameraProperties Interface");
    } 
    // Get available Sensor Modes 
    iCameraProperties->getAllSensorModes(&sensormodes); 
    if (sensormodes.size() <= 0) 
    { 
        ORIGINATE_ERROR("Preview Sensor Mode %d not available"); 
    }
    std::cout << sensormodes.size();
    ISensorMode *iSensorMode = interface_cast<ISensorMode>(sensormodes[0]); 
    if (!iSensorMode){
        ORIGINATE_ERROR("Failed to get SensorMode interface");
    } 


    // Create the capture session using the first device and get the core interface. 
    UniqueObj<CaptureSession> m_captureSession; m_captureSession.reset(iCameraProvider->createCaptureSession(g_cameraDevices[0])); 
    ICaptureSession *iCaptureSession = interface_cast<ICaptureSession>(m_captureSession); 
    if (!iCaptureSession)
    {
        ORIGINATE_ERROR("Failed to create CaptureSession");
    } 


    // Create the OutputStream. 
    UniqueObj<OutputStreamSettings> streamSettings( iCaptureSession->createOutputStreamSettings(STREAM_TYPE_EGL));


    IEGLOutputStreamSettings *iEglStreamSettings = interface_cast<IEGLOutputStreamSettings>(streamSettings); 
    if (!iEglStreamSettings) 
    {
        ORIGINATE_ERROR("Failed to create EglOutputStreamSettings"); 
    }
    iEglStreamSettings->setPixelFormat(PIXEL_FMT_YCbCr_420_888); 
    iEglStreamSettings->setMode(EGL_STREAM_MODE_MAILBOX); 
    iEglStreamSettings->setResolution(iSensorMode->getResolution()); 
    iEglStreamSettings->setMetadataEnable(true);


    UniqueObj<OutputStream> m_outputStream; 
    m_outputStream.reset(iCaptureSession->createOutputStream(streamSettings.get()));


    // Create capture request and enable the output stream. 
    UniqueObj<Request> m_request; 
    m_request.reset(iCaptureSession->createRequest()); 
    IRequest *iRequest = interface_cast<IRequest>(m_request); 
    if (!iRequest) 
    {
        ORIGINATE_ERROR("Failed to create Request"); 
    }
    iRequest->enableOutputStream(m_outputStream.get());


    UniqueObj<FrameConsumer> consumer(FrameConsumer::create(m_outputStream.get())); 
    IFrameConsumer *iFrameConsumer = interface_cast<IFrameConsumer>(consumer); 
    if(!iFrameConsumer)
    {
        ORIGINATE_ERROR("Failed to initialize Consumer");
    } 


    uint32_t requestId = iCaptureSession->capture(m_request.get()); 
    if (requestId == 0)
    {
        ORIGINATE_ERROR("Failed to submit capture request");
    } 


    Argus::Status status; 
    UniqueObj<Frame> frame(iFrameConsumer-> acquireFrame(std::chrono::seconds(5).count(), &status));


    IFrame *iFrame = interface_cast< IFrame>(frame); 
    if(!iFrame)
    {
        ORIGINATE_ERROR("Failed to get IFrame interface"); 
    } 
    Image *image = iFrame->getImage(); 
    if(!image)
    {
        ORIGINATE_ERROR("Failed to get Image from iFrame->getImage()"); 
    } 
    IImageJPEG *iImageJPEG = interface_cast<IImageJPEG>(image); 
    if(!iImageJPEG) 
    { 
        ORIGINATE_ERROR("Failed to get ImageJPEG Interface"); 
    } 
    status = iImageJPEG->writeJPEG("argus_oneShot.jpg"); 
    if(status != STATUS_OK)
    {
        ORIGINATE_ERROR("Failed to write JPEG");

    } 


    IAutoControlSettings *iAutoControlSettings = interface_cast<IAutoControlSettings>(iRequest->getAutoControlSettings()); 
    ISourceSettings *iSourceSettings = interface_cast<ISourceSettings>( iRequest->getSourceSettings());













    return EXIT_SUCCESS;
}
