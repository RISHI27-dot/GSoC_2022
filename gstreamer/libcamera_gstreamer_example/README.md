


Set the environment so that libcamerasrc gstreamer element can be found. It will be found in the libcamera build dir. Set the environment variable to the appropriate path. 
For example, I have cloned and built libcamer on the Desktop on my linux system, for me the command will be 

```
export GST_PLUGIN_PATH=~/Desktop/libcamera/build/src/gstreamer
```

To compile this example project. 
```
gcc libcamera_source_to_sink.c -o libcamera_source_to_sink `pkg-config --cflags --libs gstreamer-1.0`
```
To run the project.
```
./libcamera_source_to_sink 
```


After this the OpenGL renderer will open which will display the stream captured form the camera.

