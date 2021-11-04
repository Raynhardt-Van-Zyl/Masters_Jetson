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

#include <csignal>
#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/fill_image.h>

#include <Argus/Argus.h>
#include <EGLStream/EGLStream.h>
#include <cuda.h>
#include <cudaEGL.h>

#include  <opencv2/core/core.hpp>
#include  <opencv2/highgui/highgui.hpp>
#include  <opencv2/features2d/features2d.hpp>

#include "ArgusHelpers.h"
#include "CUDAHelper.h"
#include "EGLGlobal.h"
#include "Error.h"
#include "Thread.h"

#include "convert.h"

using namespace Argus;

/*
 * This sample opens two independent camera sessions using 2 sensors it then uses the first sensor
 * to display a preview on the screen, while taking jpeg snapshots every second from the second
 * sensor. The Jpeg saving and Preview consumption happen on two consumer threads in the
 * PreviewConsumerThread and JPEGConsumerThread classes, located in the util folder.
 */

static const uint32_t FRAMERATE = 29.999999;
static Size2D<uint32_t> STREAM_SIZE(1920, 1080);
static Range<float> GAIN_RANGE(1, 16);
static Range<float> ISP_DIGITAL_GAIN_RANGE(1, 1);
static Range<uint64_t> EXPOSURE_TIME_RANGE(13000, 683709000);

ros::Publisher left_image_pub;
ros::Publisher left_camera_info_pub;
ros::Publisher right_image_pub;
ros::Publisher right_camera_info_pub;

uint8_t *oBuffer = new uint8_t[3 * STREAM_SIZE.width() * STREAM_SIZE.height()];

namespace ArgusSamples
{

// Globals and derived constants.
EGLDisplayHolder g_display;

// Debug print macros.
#define PRODUCER_PRINT(...) printf("PRODUCER: " __VA_ARGS__)
#define CONSUMER_PRINT(...) printf("CONSUMER: " __VA_ARGS__)

class StereoConsumer : public Thread {
  public:
    explicit StereoConsumer(IEGLOutputStream *leftStream, IEGLOutputStream *rightStream)
		            : m_leftStream(leftStream), m_rightStream(rightStream)
                            , m_cuStreamLeft(NULL), m_cuStreamRight(NULL) 
	                    , m_cudaContext(0) {}
    ~StereoConsumer() {}



  private:
    virtual bool threadInitialize();
    virtual bool threadExecute();
    virtual bool threadShutdown();

    

    IEGLOutputStream *m_leftStream;
    IEGLOutputStream *m_rightStream;
    CUeglStreamConnection m_cuStreamLeft;
    CUeglStreamConnection m_cuStreamRight;
    CUcontext m_cudaContext;
};

class StereoCreator {
  public:
    explicit StereoCreator();
    ~StereoCreator();

  private:

    ICaptureSession *iCaptureSession;
    IEGLOutputStream *iStreamLeft;
    IEGLOutputStream *iStreamRight;
    UniqueObj<CameraProvider> cameraProvider;

};

class CudaFrameAcquire {
  public:
    CudaFrameAcquire(CUeglStreamConnection& connection);
    ~CudaFrameAcquire();

    bool publish(bool leftFrame);
  
  private:
    CUeglStreamConnection& m_connection;
    CUgraphicsResource m_resource;
    CUeglFrame m_frame;
    CUstream m_stream;
};

bool StereoConsumer::threadInitialize() {
  PROPAGATE_ERROR(initCUDA(&m_cudaContext));

  CONSUMER_PRINT("Connecting CUDA consumer to left stream\n");
  CUresult cuResult = cuEGLStreamConsumerConnect(&m_cuStreamLeft, m_leftStream->getEGLStream());
  if (cuResult != CUDA_SUCCESS) {
    ORIGINATE_ERROR("Unable to connect CUDA to EGLStream (%s)", getCudaErrorString(cuResult));
  }

  CONSUMER_PRINT("Connecting CUDA consumer to right stream\n");
  cuResult = cuEGLStreamConsumerConnect(&m_cuStreamRight, m_rightStream->getEGLStream());
  if (cuResult != CUDA_SUCCESS) {
    ORIGINATE_ERROR("Unable to connect CUDA to EGLStream (%s)", getCudaErrorString(cuResult));
  }
  return true;
}

bool StereoConsumer::threadExecute() {
  CONSUMER_PRINT("Waiting for Argus producer to connect to left stream.\n");
  m_leftStream->waitUntilConnected();

  CONSUMER_PRINT("Waiting for Argus producer to connect to right stream.\n");
  m_rightStream->waitUntilConnected();

  CONSUMER_PRINT("Streams connected, processing frames.\n");
  while (true) {
    EGLint streamState = EGL_STREAM_STATE_CONNECTING_KHR;

    if (!eglQueryStreamKHR(m_leftStream->getEGLDisplay(), m_leftStream->getEGLStream(),
        EGL_STREAM_STATE_KHR, &streamState) || (streamState == EGL_STREAM_STATE_DISCONNECTED_KHR)) {
      CONSUMER_PRINT("left : EGL_STREAM_STATE_DISCONNECTED_KHR received\n");
      break;
    }

    if (!eglQueryStreamKHR(m_rightStream->getEGLDisplay(), m_rightStream->getEGLStream(),
        EGL_STREAM_STATE_KHR, &streamState) || (streamState == EGL_STREAM_STATE_DISCONNECTED_KHR)) {
      CONSUMER_PRINT("right : EGL_STREAM_STATE_DISCONNECTED_KHR received\n");
      break;
    }

    CudaFrameAcquire left(m_cuStreamLeft);
    CudaFrameAcquire right(m_cuStreamRight);

    PROPAGATE_ERROR(left.publish(true) && right.publish(false));
  }
    
  CONSUMER_PRINT("No more frames. Cleaning up.\n");
  PROPAGATE_ERROR(requestShutdown());
  return true;
}

bool StereoConsumer::threadShutdown() {
  CUresult cuResult = cuEGLStreamConsumerDisconnect(&m_cuStreamLeft);
  if (cuResult != CUDA_SUCCESS) {
    ORIGINATE_ERROR("Unable to disconnect CUDA stream (%s)", getCudaErrorString(cuResult));
  }

  cuResult = cuEGLStreamConsumerDisconnect(&m_cuStreamRight);
  if (cuResult != CUDA_SUCCESS) {
    ORIGINATE_ERROR("Unable to disconnect CUDA stream (%s)", getCudaErrorString(cuResult));
  }

  PROPAGATE_ERROR(cleanupCUDA(&m_cudaContext));
  CONSUMER_PRINT("Done.\n");
  return true;
}


// CUDA stuff

CudaFrameAcquire::CudaFrameAcquire(CUeglStreamConnection& connection)
                                   : m_connection(connection)
                                   , m_stream(NULL), m_resource(0) {
  CUresult result = cuEGLStreamConsumerAcquireFrame(&m_connection, &m_resource, &m_stream, -1);
  if (result == CUDA_SUCCESS) {
    cuGraphicsResourceGetMappedEglFrame(&m_frame, m_resource, 0, 0);
  }
}

CudaFrameAcquire::~CudaFrameAcquire() {
  if (m_resource) {
    cuEGLStreamConsumerReleaseFrame(&m_connection, m_resource, &m_stream);
  }
}

bool CudaFrameAcquire::publish(bool leftFrame) {

  CUDA_RESOURCE_DESC cudaResourceDesc;
  memset(&cudaResourceDesc, 0, sizeof(cudaResourceDesc));
  cudaResourceDesc.resType = CU_RESOURCE_TYPE_ARRAY;

  cudaResourceDesc.res.array.hArray = m_frame.frame.pArray[0];
  CUsurfObject cudaSurfObj1 = 0;
  CUresult cuResult = cuSurfObjectCreate(&cudaSurfObj1, &cudaResourceDesc);
  if (cuResult != CUDA_SUCCESS) {
    ORIGINATE_ERROR("Unable to create surface object 1 (%s)", getCudaErrorString(cuResult));
  }
  
  cudaResourceDesc.res.array.hArray = m_frame.frame.pArray[1];
  CUsurfObject cudaSurfObj2 = 0;
  cuResult = cuSurfObjectCreate(&cudaSurfObj2, &cudaResourceDesc);
  if (cuResult != CUDA_SUCCESS) {
    ORIGINATE_ERROR("Unable to create surface object 2 (%s)", getCudaErrorString(cuResult));
  }

  float delta = convert(cudaSurfObj1, cudaSurfObj2, m_frame.width, m_frame.height, oBuffer);
  cuSurfObjectDestroy(cudaSurfObj1);
  cuSurfObjectDestroy(cudaSurfObj2);

  sensor_msgs::Image output;
  output.header.stamp = ros::Time::now();
  sensor_msgs::fillImage(output, sensor_msgs::image_encodings::BGR8, m_frame.height, m_frame.width, 3 * m_frame.width, (void*) oBuffer);
  //init 
  sensor_msgs::CameraInfoPtr lcam(new sensor_msgs::CameraInfo());
  // put value
  lcam->height = 1080;
  lcam->width = 1920;
  lcam->distortion_model = "plumb_bob";
  lcam->D = { -0.361976, 0.110510, 0.001014, 0.000505, 0.000000};
  lcam->K = {438.783367, 0.000000, 305.593336, 0.000000, 437.302876, 243.738352, 0.000000, 0.000000, 1.000000};
  lcam->R = {0.999978, 0.002789, -0.006046, -0.002816, 0.999986, -0.004401, 0.006034, 0.004417, 0.999972};
  lcam->P = {393.653800, 0.000000, 322.797939, 0.000000, 0.000000, 393.653800, 241.090902, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000};
  lcam->header.stamp = output.header.stamp;
  lcam->header.frame_id = output.header.frame_id;

  if (leftFrame) {
    left_image_pub.publish(output);
  } else {
    right_image_pub.publish(output);
  }
  left_camera_info_pub.publish(lcam);
  return true;
}

StereoCreator::StereoCreator() {

  printf("Started \n");
  g_display.initialize();

  cameraProvider.reset(CameraProvider::create());
  ICameraProvider *iCameraProvider = interface_cast<ICameraProvider>(cameraProvider);
  if (!iCameraProvider) {
    printf("Failed to get ICameraProvider interface");
  }
  printf("Argus Version: %s\n", iCameraProvider->getVersion().c_str());

  std::vector<CameraDevice*> cameraDevices;
  iCameraProvider->getCameraDevices(&cameraDevices);
  printf("CAMERA DEVICES COUNT: %d\n", cameraDevices.size());
  if (cameraDevices.size() < 2) {
    printf("Must have at least 2 sensors available");
  }

  std::vector <CameraDevice*> sensors;
  sensors.push_back(cameraDevices[0]);
  sensors.push_back(cameraDevices[1]);
  printf("Succesfully added sensors \n");

  // Sensor settings
  ICameraProperties *sensorDeviceProperties = interface_cast<ICameraProperties>(cameraDevices[0]);
  std::vector<Argus::SensorMode*> SensorModes;
  sensorDeviceProperties->getBasicSensorModes(&SensorModes);
  if (!SensorModes.size())
  {
      printf("Failed to get valid JPEG sensor mode list.");
  }
  ISensorMode *iSensorMode = interface_cast<ISensorMode>(SensorModes[2]);
  if (!iSensorMode)
      printf("Failed to get the sensor mode.");
    
  STREAM_SIZE = iSensorMode->getResolution();
  GAIN_RANGE = iSensorMode->getAnalogGainRange();
  EXPOSURE_TIME_RANGE = iSensorMode->getExposureTimeRange();
  printf("Succesfully got sensor settings \n");
  //End of sensor settings

  //! Capture session
  Argus::Status *status;
  UniqueObj<CaptureSession> captureSession(iCameraProvider->createCaptureSession(sensors, status));
  iCaptureSession = interface_cast<ICaptureSession>(captureSession);
  if (!iCaptureSession) {
    printf("Failed to get capture session interface \n");
  }
  printf("succesfully created capture session interface \n");

  UniqueObj<OutputStreamSettings> streamSettings(
      iCaptureSession->createOutputStreamSettings(STREAM_TYPE_EGL));
  IOutputStreamSettings *iStreamSettings =
      interface_cast<IOutputStreamSettings>(streamSettings);
  IEGLOutputStreamSettings *iEGLStreamSettings =
      interface_cast<IEGLOutputStreamSettings>(streamSettings);
  if (!iStreamSettings || !iEGLStreamSettings) {
    printf("Failed to create OutputStreamSettings");
  }
  
  iEGLStreamSettings->setPixelFormat(PIXEL_FMT_YCbCr_420_888);
  iEGLStreamSettings->setResolution(STREAM_SIZE);
  iEGLStreamSettings->setEGLDisplay(g_display.get());
  iEGLStreamSettings->setMode(EGL_STREAM_MODE_MAILBOX);
  iEGLStreamSettings->setMetadataEnable(true);

  printf("Succesfully created stream settings \n");

  iStreamSettings->setCameraDevice(sensors[0]);
  UniqueObj<OutputStream> streamLeft(iCaptureSession->createOutputStream(streamSettings.get()));
  iStreamLeft = interface_cast<IEGLOutputStream>(streamLeft);
  if (!iStreamLeft) {
    printf("Failed to create left stream");
  }
  printf("Succesfully added left stream \n");
  iStreamSettings->setCameraDevice(sensors[1]);
  UniqueObj<OutputStream> streamRight(iCaptureSession->createOutputStream(streamSettings.get()));
  iStreamRight = interface_cast<IEGLOutputStream>(streamRight);
  if (!iStreamRight) {
    printf("Failed to create right stream");
  }
  printf("Succesfully added right stream\n");

  UniqueObj<Request> request(iCaptureSession->createRequest());
  IRequest *iRequest = interface_cast<IRequest>(request);
  if (!iRequest) {
    printf("Failed to create Request");
  }

  iRequest->enableOutputStream(streamLeft.get());
  iRequest->enableOutputStream(streamRight.get());

  ISourceSettings *iSourceSettings = interface_cast<ISourceSettings>(request);
  if (!iSourceSettings) {
    printf("Failed to get source settings request interface");
  }
  iSourceSettings->setFrameDurationRange(Range<uint64_t>(1e9 / FRAMERATE));
  iSourceSettings->setExposureTimeRange(EXPOSURE_TIME_RANGE);
  iSourceSettings->setGainRange(GAIN_RANGE);
  
  IAutoControlSettings *iAutoControlSettings = 
	  interface_cast<IAutoControlSettings>(iRequest->getAutoControlSettings());
  iAutoControlSettings->setIspDigitalGainRange(ISP_DIGITAL_GAIN_RANGE);


  PRODUCER_PRINT("Launching disparity checking consumer\n");
  StereoConsumer disparityConsumer(iStreamLeft, iStreamRight);
  disparityConsumer.initialize();
  disparityConsumer.waitRunning();

  PRODUCER_PRINT("Starting repeat capture requests.\n");
  if (iCaptureSession->repeat(request.get()) != STATUS_OK) {
    printf("Failed to start repeat capture request for preview");
  }

}

StereoCreator::~StereoCreator(){
  iCaptureSession->stopRepeat();
  iCaptureSession->waitForIdle();

  PRODUCER_PRINT("Captures complete, disconnecting producer.\n");
  iStreamLeft->disconnect();
  iStreamRight->disconnect();

  cameraProvider.reset();
  g_display.cleanup();

  PRODUCER_PRINT("Done -- exiting.\n");
  ros::shutdown();
}



}; // namespace ArgusSamples

int main(int argc, char **argv)
{
  ros::init(argc, argv, "argus_stereo_node");
  ros::NodeHandle nh;

  left_image_pub = nh.advertise<sensor_msgs::Image>("/camera/left/image", 1);
  left_camera_info_pub = nh.advertise<sensor_msgs::CameraInfo>("/camera/left/camera_info", 1);
  right_image_pub = nh.advertise<sensor_msgs::Image>("/camera/right/image", 1);
  right_camera_info_pub = nh.advertise<sensor_msgs::CameraInfo>("/camera/right/camera_info", 1);

  ArgusSamples::StereoCreator stereo;
  ros::spin();
  return 0;
}
