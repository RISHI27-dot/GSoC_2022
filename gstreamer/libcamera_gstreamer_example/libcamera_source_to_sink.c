#include <gst/gst.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  GstElement *pipeline, *source, *sink, *filter;
  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;

  // Initialize GStreamer
  gst_init(&argc, &argv);

  // Creating required elements
  source = gst_element_factory_make("libcamerasrc", "source");
  filter = gst_element_factory_make("videoflip", "filter");
  sink = gst_element_factory_make("glimagesink", "sink");
  // List the supported properties of libcamerasrc
  GObjectClass *objClass = G_OBJECT_GET_CLASS(source);
  guint n_props;
  GParamSpec **props;
  props = g_object_class_list_properties(objClass, &n_props);
  printf("available properties: ");
  if (objClass != NULL)
  {
    if (props != NULL)
    {
      for (guint i = 0; i < n_props; i++)
      {
        if (props[i] != NULL)
        {
          printf("%s, ", props[i]->name);
        }
      }
      printf("\n");
    }
    else
    {
      printf("Element props are NULL!");
    }
  }
  else
  {
    printf("Obj Class is NULL!");
  }

  // Creating pipeline
  pipeline = gst_pipeline_new("my-pipeline");

  if (!pipeline || !source || !filter || !sink)
  {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }

  // adding and linking the element and building the pipeline
  gst_bin_add_many(GST_BIN(pipeline), source, filter, sink, NULL);
  if (gst_element_link(source, filter) != TRUE)
  {
    g_printerr("Source and filter could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }
  if (gst_element_link(filter, sink) != TRUE)
  {
    g_printerr("filter and sink could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  // setting the filter to rotate the video streame clockwise by 90 degrees.
  g_object_set(filter, "method", 0, NULL);
  // gchar *cam_name;
  // g_object_get (source,"camera-name",&cam_name,NULL);
  // if(cam_name == NULL){
  //   printf("the camera name is null");
  // }else{
  //   printf("%s",cam_name);
  // }
  // start playing the pipeline
  ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE)
  {
    g_printerr("Unable to set the pipeline to the playing state.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  bus = gst_element_get_bus(pipeline);
  msg =
      gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                 GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  if (msg != NULL)
  {
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_ERROR:
      gst_message_parse_error(msg, &err, &debug_info);
      g_printerr("Error received from element %s: %s\n",
                 GST_OBJECT_NAME(msg->src), err->message);
      g_printerr("Debugging information: %s\n",
                 debug_info ? debug_info : "none");
      g_clear_error(&err);
      g_free(debug_info);
      break;
    case GST_MESSAGE_EOS:
      g_print("End-Of-Stream reached.\n");
      break;
    default:
      g_printerr("Unexpected message received.\n");
      break;
    }
    gst_message_unref(msg);
  }

  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
  return 0;
}
