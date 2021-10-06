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

using namespace Argus;

/*
 * This sample opens two independent camera sessions using 2 sensors it then uses the first sensor
 * to display a preview on the screen, while taking jpeg snapshots every second from the second
 * sensor. The Jpeg saving and Preview consumption happen on two consumer threads in the
 * PreviewConsumerThread and JPEGConsumerThread classes, located in the util folder.
 */

namespace ArgusSamples
{

// Globals and derived constants.
EGLDisplayHolder g_display;

// Debug print macros.
#define PRODUCER_PRINT(...) printf("PRODUCER: " __VA_ARGS__)

/*******************************************************************************
 * Extended options class to add additional options specific to this sample.
 ******************************************************************************/
class MultiSensorSampleOptions : public CommonOptions
{
public:
    MultiSensorSampleOptions(const char *programName)
        : CommonOptions(programName,
                        ArgusSamples::CommonOptions::Option_D_CameraDevice |
                        ArgusSamples::CommonOptions::Option_R_WindowRect |
                        ArgusSamples::CommonOptions::Option_T_CaptureTime)
        , m_jpegDeviceIndex(1)
    {
        addOption(createValueOption
            ("jpeg", 'j', "INDEX", "Camera device index of JPEG stream "
             "(use --device to set device index for preview stream).", m_jpegDeviceIndex));
    }

    uint32_t jpegDeviceIndex() const { return m_jpegDeviceIndex.get(); }

protected:
    Value<uint32_t> m_jpegDeviceIndex;
};

static bool execute(const MultiSensorSampleOptions& options)
{
    // Initialize the window and EGL display.
    
    // ! leave this
    
    Window &window = Window::getInstance();
    window.setWindowRect(options.windowRect());
    PROPAGATE_ERROR(g_display.initialize(window.getEGLNativeDisplay()));
    

    // Initialize the Argus camera provider.
    UniqueObj<CameraProvider> cameraProvider(CameraProvider::create());

    // Get the ICameraProvider interface from the global CameraProvider.
    ICameraProvider *iCameraProvider = interface_cast<ICameraProvider>(cameraProvider);
    if (!iCameraProvider)
        ORIGINATE_ERROR("Failed to get ICameraProvider interface");
    printf("Argus Version: %s\n", iCameraProvider->getVersion().c_str());

    // Get the camera devices.
    std::vector<CameraDevice*> cameraDevices;
    iCameraProvider->getCameraDevices(&cameraDevices);

    Argus::CameraDevice* sensor1 = cameraDevices[0];
    Argus::CameraDevice* sensor2 = cameraDevices[1];

    // Get the default SensorMode from the JPEG device to determine the stream size
    // (the window rect size is used for the preview stream size).

    // Sensor 1
        ICameraProperties *sensor1DeviceProperties = interface_cast<ICameraProperties>(sensor1);
        if (!sensor1DeviceProperties)
        {
            ORIGINATE_ERROR("Failed to get the JPEG camera device properties interface.");
        }
        std::vector<Argus::SensorMode*> Sensor1Modes;
        sensor1DeviceProperties->getBasicSensorModes(&Sensor1Modes);
        if (!Sensor1Modes.size())
        {
            ORIGINATE_ERROR("Failed to get valid JPEG sensor mode list.");
        }
        ISensorMode *iSensor1Mode = interface_cast<ISensorMode>(Sensor1Modes[2]);
        if (!iSensor1Mode)
            ORIGINATE_ERROR("Failed to get the sensor mode.");

    // Create the JPEG capture session for sensor 1.
        UniqueObj<CaptureSession> jpegSession1 = UniqueObj<CaptureSession>(
                iCameraProvider->createCaptureSession(sensor1));
        if (!jpegSession1)
            ORIGINATE_ERROR(
                "Failed to create JPEG session with camera index %d.", options.jpegDeviceIndex());
        ICaptureSession *iJpegCaptureSession1 = interface_cast<ICaptureSession>(jpegSession1);
        if (!iJpegCaptureSession1)
            ORIGINATE_ERROR("Failed to get JPEG capture session interface");


    // Create JPEG stream for sensor 1.
        PRODUCER_PRINT("Creating the JPEG stream.\n");
        UniqueObj<OutputStreamSettings> jpegSettings1(
            iJpegCaptureSession1->createOutputStreamSettings(STREAM_TYPE_EGL));
        IEGLOutputStreamSettings *iJpegSettings1 =
            interface_cast<IEGLOutputStreamSettings>(jpegSettings1);
        if (iJpegSettings1)
        {
            iJpegSettings1->setPixelFormat(PIXEL_FMT_YCbCr_420_888);
            iJpegSettings1->setResolution(iSensor1Mode->getResolution());
            iJpegSettings1->setEGLDisplay(g_display.get());
            iJpegSettings1->setMetadataEnable(true);
        }
        UniqueObj<OutputStream> jpegStream1(
                iJpegCaptureSession1->createOutputStream(jpegSettings1.get()));
        if (!jpegStream1.get())
            ORIGINATE_ERROR("Failed to create JPEG OutputStream");

        JPEGConsumerThread jpegConsumer1(jpegStream1.get());
        PROPAGATE_ERROR(jpegConsumer1.initialize());
        PROPAGATE_ERROR(jpegConsumer1.waitRunning());

    
    


    //Sensor 2
        ICameraProperties *sensor2DeviceProperties = interface_cast<ICameraProperties>(sensor2);
        if (!sensor2DeviceProperties)
        {
            ORIGINATE_ERROR("Failed to get the JPEG camera device properties interface.");
        }
        std::vector<Argus::SensorMode*> Sensor2Modes;
        sensor2DeviceProperties->getBasicSensorModes(&Sensor2Modes);
        if (!Sensor2Modes.size())
        {
            ORIGINATE_ERROR("Failed to get valid JPEG sensor mode list.");
        }
        ISensorMode *iSensor2Mode = interface_cast<ISensorMode>(Sensor2Modes[2]);
        if (!iSensor2Mode)
            ORIGINATE_ERROR("Failed to get the sensor mode.");


    // Create the JPEG capture session for sensor 2.
        UniqueObj<CaptureSession> jpegSession2 = UniqueObj<CaptureSession>(
                iCameraProvider->createCaptureSession(sensor2));
        if (!jpegSession2)
            ORIGINATE_ERROR(
                "Failed to create JPEG session with camera index %d.", options.jpegDeviceIndex());
        ICaptureSession *iJpegCaptureSession2 = interface_cast<ICaptureSession>(jpegSession2);
        if (!iJpegCaptureSession2)
            ORIGINATE_ERROR("Failed to get JPEG capture session interface");


    // Create JPEG stream for sensor 1.
        PRODUCER_PRINT("Creating the JPEG stream.\n");
        UniqueObj<OutputStreamSettings> jpegSettings2(
            iJpegCaptureSession2->createOutputStreamSettings(STREAM_TYPE_EGL));
        IEGLOutputStreamSettings *iJpegSettings2 =
            interface_cast<IEGLOutputStreamSettings>(jpegSettings2);
        if (iJpegSettings2)
        {
            iJpegSettings2->setPixelFormat(PIXEL_FMT_YCbCr_420_888);
            iJpegSettings2->setResolution(iSensor2Mode->getResolution());
            iJpegSettings2->setEGLDisplay(g_display.get());
            iJpegSettings2->setMetadataEnable(true);
        }
        UniqueObj<OutputStream> jpegStream2(
                iJpegCaptureSession2->createOutputStream(jpegSettings2.get()));
        if (!jpegStream2.get())
            ORIGINATE_ERROR("Failed to create JPEG OutputStream");

        JPEGConsumerThread jpegConsumer2(jpegStream2.get());
        PROPAGATE_ERROR(jpegConsumer2.initialize());
        PROPAGATE_ERROR(jpegConsumer2.waitRunning());






    // Create the two requests
    UniqueObj<Request> jpegRequest1(iJpegCaptureSession1->createRequest());
    UniqueObj<Request> jpegRequest2(iJpegCaptureSession2->createRequest());

    IRequest *iJpegRequest1 = interface_cast<IRequest>(jpegRequest1);
    IRequest *iJpegRequest2 = interface_cast<IRequest>(jpegRequest2);

    iJpegRequest1->enableOutputStream(jpegStream1.get());
    iJpegRequest2->enableOutputStream(jpegStream2.get());

    // Argus is now all setup and ready to capture

    // Submit capture requests.
    PRODUCER_PRINT("Starting repeat capture requests.\n");

    // Wait for the specified number of seconds and do a JPEG capture every second.
    for (uint32_t i = 0; i < 2; i++)
    {
        if (iJpegCaptureSession1->capture(jpegRequest1.get()) == 0)
            ORIGINATE_ERROR("Failed to submit JPEG capture request");

        if (iJpegCaptureSession2->capture(jpegRequest2.get()) == 0)
            ORIGINATE_ERROR("Failed to submit JPEG capture request");
        PROPAGATE_ERROR(window.pollEvents());
        sleep(1);
    }
    window.pollEvents();

    // all done shut down
    iJpegCaptureSession1->stopRepeat();
    iJpegCaptureSession1->waitForIdle();

    iJpegCaptureSession2->stopRepeat();
    iJpegCaptureSession2->waitForIdle();

    jpegStream1.reset();
    jpegStream2.reset();

    // Wait for the consumer threads to complete.
    PROPAGATE_ERROR(jpegConsumer1.shutdown());
    PROPAGATE_ERROR(jpegConsumer2.shutdown());

    // Shut down Argus.
    cameraProvider.reset();

    // Shut down the window (destroys window's EGLSurface).
    window.shutdown();

    // Cleanup the EGL display
    PROPAGATE_ERROR(g_display.cleanup());

    PRODUCER_PRINT("Done -- exiting.\n");
    return true;
}

}; // namespace ArgusSamples

int main(int argc, char** argv)
{
    ArgusSamples::MultiSensorSampleOptions options(basename(argv[0]));
    if (!options.parse(argc, argv))
        return EXIT_FAILURE;
    if (options.requestedExit())
        return EXIT_SUCCESS;

    if (!ArgusSamples::execute(options))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
