# Teenie Harris Project

---
### Teenie Harris Files

* ```canonical_filename_order.txt``` [**[1.1MB .TXT](canonical_filename_order.txt)**]
  * 59278 rows listing the "canonical order" we are using for the Teenie Harris image filenames.

---
### Imagga API

*Analysis of the Teenie Harris archive using the [Imagga](https://imagga.com/) image analysis service, including a wide range of semantic descriptors and confidence values.*

* ```imagga_analyses_of_teenie_harris_archive.zip``` [[**106MB ZIP**](https://storage.googleapis.com/teenieharris/imagga/json/imagga_analyses_of_teenie_harris_archive.zip)]
  * Zip archive containing 59,190 JSON files. Input images were 1600 pixels in their maximum dimension.
  * [Example JSON file](imagga/json/result_Box_100_15974.png.json).

---
### Google Vision API

*Results of analysis using the Google Vision API on the TeenieHarris 1600-pixel PNG images. Information includes face landmarks, object recognition, facial expression analysis, and optical character recognition (OCR).*

* ```google_analyses_of_teenie_harris_archive.zip``` [[**321.21MB .ZIP**](https://storage.googleapis.com/teenieharris/google/json/google_analyses_of_teenie_harris_archive.zip)]
  * ZIP archive containing 59,278 JSON files, each describing a corresponding  Teenie Harris image. Input images were 1600 pixels in their maximum dimension. 
  * [Example JSON file](google/json/visionResult_Box100_15974.json).


---
### Detectron

*Distillation of multi-category object detection using the [Facebook Detectron/COCO](https://github.com/facebookresearch/Detectron) object detector.*

* ```detectron_categories.txt``` [[**1kb .TXT**](detectron/detectron_categories.txt)]
  * The ordered list of 80 Detectron/COCO categories, taken from [here](https://gist.github.com/AruniRC/7b3dadd004da04c80198557db5da4bda).
* ```teenie_detectron_json.zip``` [[**2.36GB ZIP**](https://storage.googleapis.com/teenieharris/detectron/json/teenie_detectron_json.zip)]
  * Zip archive containing 58696 JSON files. Each JSON file containins contours of objects (and their category labels) calculated by the Facebook Detectron system. Contour vertex coordinates are scaled to input images whose maximum dimension was 1600 pixels. 
  * [Example JSON file](detectron/json/15974.json)
* ```detectron_rendered_contours_1600px.zip``` [[**583.77MB .ZIP**](https://storage.googleapis.com/teenieharris/detectron/png1600/detectron_rendered_contours_1600px.zip)]
  * Zip archive containing 58696 .PNG files, each of which displays (in white pixels on a black background) the contours of people (exclusively) in the correspondingly-named Teenie Harris image. Only contours of people are shown. All images are scaled to 1600 pixels in their maximum dimension.
  * [Example PNG file](detectron/png1600/15974.png)
* ```canonical_detectron_category_scores_59278rows.tsv``` [[**1.24MB ZIP**](https://storage.googleapis.com/teenieharris/detectron/tsv/canonical_detectron_category_scores_59278rows.tsv.zip)]
  * This file reports, for every Teenie Harris image, the confidence scores (0...1) that each of the Detectron categories appears in that image.
  * In cases where an image contains more than one detected object with that class category, this file reports the *maximum* confidence score encountered in that image. 
  * The rows of this file are in "canonical order", meaning, the sorted according to the *canonical list of files*. This canonical list (generated via 'natural sorting' can be found [here](processing_code/DetectronCategorizerReorder/data/canonical_filename_order.txt).
  * Zero'ed rows for the 'missing' (null-detectron-data) 582 images have been appropriately inserted, so that the number of rows is 59278. 
  * Image filenames are *not* included in this file. 
  * This is a zipped file of tab-separated values (TSV). 
* ```canonical_detectron_category_counts_59278rows.tsv``` [[**439kb .ZIP**](https://storage.googleapis.com/teenieharris/detectron/tsv/canonical_detectron_category_counts_59278rows.tsv.zip)]
  * Same as the above, but this file reports, for every image, a row containing the *count of instances* for each Detectron category.
* ```detectron_category_scores_boxorder_58696rows.tsv``` [[**1.39MB .ZIP**](https://storage.googleapis.com/teenieharris/detectron/tsv/detectron_category_scores_boxorder_58696rows.tsv.zip)]
  * This file reports, for every Teenie Harris image, the confidence scores (0...1) that each of the Detectron categories appears in that image.
  * In cases where an image contains more than one detected object with that class category, this file reports the *maximum* confidence score encountered in that image.  
  * The first column is the file name of the image, e.g. "Box_001/686.json". 
  * The rows of this file are in "box order", meaning, the order produced by Java/Processing when recursively traversing the Teenie Harris image collection.  
  * This file only contains 58696 rows. 582 images do not have any Detectron results nor any corresponding files.
  * This is a zipped file of tab-separated values (TSV).
* ```detectron_category_counts_boxorder_58696rows.tsv``` [[**595kb .ZIP**](https://storage.googleapis.com/teenieharris/detectron/tsv/detectron_category_counts_boxorder_58696rows.tsv.zip)]
  * Same as the above, but this file reports the *count of instances* for each category. An image with 3 cars will have the integer '3' in the column corresponding to automobiles.

### MORE

