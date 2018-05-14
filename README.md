# Tom

This is a collection of my daily projects, which are organized in separate files. The propose of each project states as following.

## BGMerge

Merge the Background from the warped left view and the warped right view, considering their depth value.

<details>
<summary>IO :</summary>

  - Input : 
    - Left View Warped Background Color Image
	- Left View Warped Background Depth Map
	- Right View Warped Background Color Image
	- Right View Warped Background Depth Map
  - Output : 
    - Merged Virtual View Background Color Image
	- Merged Virtual View Background Depth Map
	
</details>

## BackgroundEstimation

Estimate the background through Depth Map, which considers the object distance to camera.

<details>
<summary>IO :</summary>

  - Input : 
    - A Certain View Color Image
	- A Certain View Depth Map
  - Output : 
    - Certain View Background Color Image
	- Certain View Background Depth Map
	
</details>

## NBU

Temporal File of NBU OJ.

[NBU Online Judge](http://www.nbuoj.com/v8.8/Home/Home.php)

## OpenCVTest

Test case of OpenCV.

  *Function List* 
  
  - HISTOGRAM
  - SIFT_AND_BFMATCHER
  - SURF_AND_BFMATCHER
  - SURF_AND_FLANN
  - SURF_AND_FLANN_AND_OBJECT_DETECTION
  - ORB_AND_FLANN
  - ERODE
  - DILATE
  - ADVANCED_MORPHOLOGY
  - RESIZE
  - PYRAMID
  - BINARYZATION
  - MEAN_FILTERING
  - CANNY_EDGE_DETECTION
  - YUV_CLASS
  - WATERSHED
  - INPAINT
  
## Pythontest

Coding Python in *Visual Stdio*. Test the example of *mnist*.

## ReadTest

Realize the function to *Read Parameter from File*.

## ViewSynthesis

My DIBR Project, including three functions: 

  - ForwardWarp
  - Merge
  - FillHoles
  
## YUV_Test

Modify the *CIYuv* class, and use two ways to extract color background(BACKGROUND_AVERAGE & BACKGROUND_GMM), finally apply it to fill the holes from single view rendering.
