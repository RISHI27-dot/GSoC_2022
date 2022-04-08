#include<iomanip>
#include <iostream>
#include <memory>

#include <libcamera/libcamera.h>

#include "event_loop.h"
#include <libcamera/file_sink.h>


#define TIMEOUT_SEC 3

using namespace libcamera;
static std::shared_ptr<Camera> camera;
static EventLoop loop;

static void process_request(Request* request);
//callback functio for the event that the frame buffer passed to the camera device has
// been filled by the camera device
static void request_completed(Request* request)
{
    if(request->status() == Request::RequestCancelled){
        return;
    }

	loop.callLater(std::bind(&process_request, request));
}
static void process_request(Request* request){

}
int main(){

    //making a cmera mangager object
    //camera manager is responsible for handling the cameras in the system
    std::unique_ptr<CameraManager> cm = std::make_unique<CameraManager>();
    cm->start();

    //display the list of cameras found in the system iterating over all the cameras
    for(auto const &camera : cm->cameras()){
        std::cout<< "Cameras found in the system: "<< camera.get()->id()<<std::endl;
    }

    //stop the camera manager if there are no cameras in the system i.e cameras() function
    //returned an empty vector
    if(cm->cameras().empty()){
        std::cout<<"no cameras found on the system stopping the camera manager."<<std::endl;
        cm->stop();
        return 1;
    }

    //to use the camera the application needs to 'aquire' the camera
    std::string camra_id = cm->cameras()[0]->id();
    camera = cm->get(camra_id);
    camera->acquire();

    //genrate the camera configuration, here we will use the VideoRecording streamrole
    std::unique_ptr<CameraConfiguration> config = 
        camera->generateConfiguration({StreamRole::VideoRecording});

    // Creating StreamConfiguration instence fot the StreamRole VideoRecording
    StreamConfiguration &stream_config = config->at(0);
    // displaying the configuration on the console
    std::cout<<"current configuration is: "<< stream_config.toString()<<std::endl;
    //validating the configuration before it cab be applied to the camera
    config->validate();
    std::cout<<"validated cofniguration: "<<stream_config.toString()<<std::endl;
    //applying the validated configuration to the camera
    camera->configure(config.get());

    //allocating frame buffers form libcamra to the appliction.
    FrameBufferAllocator *allocator = new FrameBufferAllocator(camera);

    for (StreamConfiguration &cfg : *config){
        int ret = allocator->allocate(cfg.stream());
        if (ret<0){
            std::cout<<"buffer alloation failed"<<std::endl;
            return 1;
        }
        size_t allocated = allocator->buffers(cfg.stream()).size();
        std::cout<<"allocated "<<allocated<<" buffers for the stream"<<std::endl;
    }

    //make a stream object. streams are exposed by the camera object of the class Camera
    Stream *stream = stream_config.stream();
    //creating a vector to store the buffers corresponding to a stream.
    const std::vector<std::unique_ptr<FrameBuffer>> &buffers = allocator->buffers(stream);
    //creating a vector of requests
    std::vector<std::unique_ptr<Request>> requests;
	for (unsigned int i = 0; i < buffers.size(); ++i) {
        //ceating a new request 
		std::unique_ptr<Request> request = camera->createRequest();
		if (!request)
		{
			std::cerr << "Can't create request" << std::endl;
			return EXIT_FAILURE;
		}
        //assigning buffer to the request
        const std::unique_ptr<FrameBuffer> &buffer = buffers[i];
        int ret = request->addBuffer(stream, buffer.get());
        if(ret < 0)
        {
            std::cout<<"cannot set buffer for the request"<<std::endl;
            return 1;
        }
        //storing the request in the vector of request
        ControlList &controls = request->controls();
        controls.set(controls::Brightness,0.5);
        requests.push_back(std::move(request));
    }

	//  * Signals are events 'emitted' by a class instance.
	//  * Slots are callbacks that can be 'connected' to a Signal.
    // first we connect the slot

    camera->requestCompleted.connect(request_completed);

    //finally start capturing frames

    camera->start();
    //queue the request stored in the requests vector into the camera
	for (std::unique_ptr<Request> &request : requests)
		camera->queueRequest(request.get());

    //to manage the events and dispatch them use event loop
    loop.timeout(TIMEOUT_SEC);
    int ret = loop.exec();
    std::cout<<ret<<std::endl;

    camera->stop();
	allocator->free(stream);
	delete allocator;
	camera->release();
	camera.reset();
	cm->stop();

	return EXIT_SUCCESS;
}

