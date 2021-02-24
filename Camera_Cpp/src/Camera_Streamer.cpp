/*
  Example code for displaying gstreamer video from the CSI port of the Nvidia Jetson in OpenCV.
  Created by Peter Moran on 7/29/17.
  https://gist.github.com/peter-moran/742998d893cd013edf6d0c86cc86ff7f
*/

#include <iostream>
#include <string.h>
#include <Argus/Argus.h>
#include "Thread.h"

std::string get_tegra_pipeline(int width, int height, int fps) {
    return "nvarguscamerasrc ! nvvidconv flip-method=2 ! video/x-raw(memory:NVMM), width=1280, height=720,format=NV12, framerate=120/1 ! nvvidconv ! video/x-raw,format=I420 ! appsink";
}

int main() {


    
    // Initialize the Argus camera provider. 
    UniqueObj<CameraProvider> g_cameraProvider; 
    g_cameraProvider = UniqueObj<CameraProvider> (CameraProvider::create()); 


    // Get the ICameraProvider interface from the global CameraProvider 
    ICameraProvider *iCameraProvider = interface_cast<ICameraProvider>(g_cameraProvider); 
    if (!iCameraProvider) 
        ORIGINATE_ERROR("Failed to get ICameraProvider interface");
    
    
    // Get the camera devices. 
    std::vector<CameraDevice*> g_cameraDevices; 
    iCameraProvider->getCameraDevices(&g_cameraDevices); 
    if (g_cameraDevices.size() == 0) 
    { 
        ORIGINATE_ERROR("No cameras detected"); 
    }else
    {
        cout << "Number of cameras: " << (unsigned)g_cameraDevices.size()
    }
    

    // Camera properties of camera 
    std::vector<SensorMode*> sensormodes; 
    ICameraProperties *iCameraProperties = interface_cast<ICameraProperties>(g_cameraDevices[0]); 
    if (!iCameraProperties) 
        ORIGINATE_ERROR("Failed to get ICameraProperties Interface of camera 0"); 
    

    // Get available Sensor Modes 
    iCameraProperties->getAllSensorModes(&sensormodes); 
    if (sensormodes.size() <= options.sensorModeIndex()) 
    { 
        ORIGINATE_ERROR("Preview Sensor Mode not available"); 
    }


    ISensorMode *iSensorMode = interface_cast<ISensorMode>(sensormodes[options.sensorModeIndex()]);
    if (!iSensorMode) 
        ORIGINATE_ERROR("Failed to get SensorMode interface");

    
    // Create the capture session using the first device and get the core interface. 
    UniqueObj<CaptureSession> m_captureSession;
    
    m_captureSession.reset(iCameraProvider->createCaptureSession(g_cameraDevices[0])); 
    ICaptureSession *iCaptureSession = interface_cast<ICaptureSession>(m_captureSession); 
    if (!iCaptureSession) 
        ORIGINATE_ERROR("Failed to create CaptureSession");


    // Create the OutputStream. 
    UniqueObj<OutputStreamSettings> streamSettings( iCaptureSession->createOutputStreamSettings(STREAM_TYPE_EGL))


    IEGLOutputStreamSettings *iEglStreamSettings = interface_cast<IEGLOutputStreamSettings>(streamSettings); 
    if (!iEglStreamSettings) 
        ORIGINATE_ERROR("Failed to create EglOutputStreamSettings"); 
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
    if (!iRequest) ORIGINATE_ERROR("Failed to create Request"); 
    iRequest->enableOutputStream(m_outputStream.get());


    UniqueObj<FrameConsumer> consumer(FrameConsumer::create(m_outputStream.get())); 
    IFrameConsumer *iFrameConsumer = interface_cast<IFrameConsumer>(consumer); 
    if(!iFrameConsumer) 
        ORIGINATE_ERROR("Failed to initialize Consumer");
    

    uint32_t requestId = iCaptureSession->capture(m_request.get()); 
    if (requestId == 0) 
        ORIGINATE_ERROR("Failed to submit capture request");
    

    Status status; 
    UniqueObj<Frame> frame(iFrameConsumer-> acquireFrame(FIVE_SECONDS_IN_NANOSECONDS, &status));


    IFrame *iFrame = interface_cast< IFrame>(frame); 
    if(!iFrame) ORIGINATE_ERROR("Failed to get IFrame interface");
        Image *image = iFrame->getImage();
    if(!image) ORIGINATE_ERROR("Failed to get Image from iFrame->getImage()");
        IImageJPEG *iImageJPEG = interface_cast<IImageJPEG>(image);
    if(!iImageJPEG) 
    { 
        ORIGINATE_ERROR("Failed to get ImageJPEG Interface");
    } 
    status = iImageJPEG->writeJPEG("argus_oneShot.jpg");
    if(status != STATUS_OK) 
        ORIGINATE_ERROR("Failed to write JPEG");












    // Options
    int WIDTH = 1920;
    int HEIGHT = 1080;
    int FPS = 30;

    // Define the gstream pipeline
    std::string pipeline = get_tegra_pipeline(WIDTH, HEIGHT, FPS);
    std::cout << "Using pipeline: \n\t" << pipeline << "\n";

    // Create OpenCV capture object, ensure it works.
    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
    if (!cap.isOpened()) {
        std::cout << "Connection failed";
        return -1;
    }

    // View video
    cv::Mat frame;
    while (1) {
        cap >> frame;  // Get a new frame from camera

        // Display frame
        imshow("Display window", frame);
        cv::waitKey(1); //needed to show frame
    }
}