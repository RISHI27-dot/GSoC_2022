

# Introduction
After going through the gstreamer tutorials, I have made a test application that uses the libcamera gstreamer element to capture video from the camera and displays the stream on the OpenGL renderer. Between the source (libcamerasrc) and the sink (glimagesink) I have added a filter (videoflip) to flip the video stream clockwise by 90 degrees.

# Usage
Set the environment so that libcamerasrc gstreamer element can be found.
For example, I have cloned and built libcamera on the Desktop on my linux system, for me the command to set the environment will be 

```
export GST_PLUGIN_PATH=~/Desktop/libcamera/build/src/gstreamer
```

To compile the example project. 
```
gcc libcamera_source_to_sink.c -o libcamera_source_to_sink `pkg-config --cflags --libs gstreamer-1.0`
```
To run the example project.
```
./libcamera_source_to_sink 
```


After this the OpenGL renderer will be opened which will display the stream captured form the camera.

