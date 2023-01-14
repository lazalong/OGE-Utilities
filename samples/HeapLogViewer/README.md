# Heap Log Viewer

## Features

 - Visualise log heap allocation/deallocation/clearing events
 - Input: 
   - JSON log format
   - TODO: compact log txt format 
   - TODO: compact log binary format

 ## Design

 - Parse the whole log to determine the max amount of memory space used
 - Step-by-step display of events
 - Display other log events

 - As the Canvas has a size of 1024*500 pixels, then if 1 pixel is 1 byte we can display 500 KB.
   For 2GB, each pixel is 2^31 / 1024 /500  = 2'147'483'648 / 512000 = 4194.304.


 ## 


   