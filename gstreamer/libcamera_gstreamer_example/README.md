


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


After this the OpenGL renderer will open which will display the stream captured form the camera.

