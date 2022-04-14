


Set the environmet so that libcamerasrc gstreamer element can be found. It will be found in the libcamera build dir. Set the environment variable to appropriate path. 
For example, I have cloned and buit libcamer on the Desktop on my linux system, for me the command will be 

```
export GST_PLUGIN_PATH=export GST_PLUGIN_PATH=~/Desktop/libcamera/build/src/gstreamer

```

To build this example run

```
gcc libcamera_source_to_sink.c -o libcamera_source_to_sink `pkg-config --cflags --libs gstreamer-1.0`
```
and now run

`./libcamera_source_to_sink `

After this the OpenGL renderer will open which will display the stream captured form the camera.

