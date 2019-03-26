### Detectron/COCO Categories in Teenie Harris images

Distillation of multi-category object detection using the [Facebook Detectron/COCO](https://github.com/facebookresearch/Detectron) object detector.

**Primary files:**

* ```detectron_categories.txt``` 
  * The list of 80 Detectron/COCO categories, taken from [here](https://gist.github.com/AruniRC/7b3dadd004da04c80198557db5da4bda).
* ```detectron_category_scores_boxorder_58696rows.tsv``` 
  * This file reports, for every Teenie Harris image, the confidence scores (0...1) that each of the Detectron categories appears in that image.
  * In cases where an image contains more than one detected object with that class category, this file reports the *maximum* confidence score encountered in that image.  
  * The first column is the file name of the image, e.g. "Box_001/686.json". 
  * The rows of this file are in "box order", meaning, the order produced by Java/Processing when recursively traversing the Teenie Harris image collection.  
  * This file only contains 58696 rows. Some 582 images are missing Detectron results. 
  * This is a file of tab-separated values (TSV).
* ```detectron_category_counts_boxorder_58696rows.tsv``` 
  * Same as the above, but this file reports the *count of instances* for each category. An image with 3 cars will have the integer '3' in the column corresponding to automobiles. 
* ```canonical_detectron_category_scores_59278rows.tsv```
  * This file reports, for every Teenie Harris image, the confidence scores (0...1) that each of the Detectron categories appears in that image.
  * In cases where an image contains more than one detected object with that class category, this file reports the *maximum* confidence score encountered in that image. 
  * The rows of this file are in "canonical order", meaning, the sorted according to the *canonical list of files*. This canonical list (generated via 'natural sorting' can be found [here](processing_code/DetectronCategorizerReorder/data/canonical_filename_order.txt).
  * Zero'ed rows for the missing 582 images have been appropriately inserted, so that the number of rows is 59278. 
  * Image filenames are *not* included in this file. 
  * This is a file of tab-separated values (TSV). 
* ```canonical_detectron_category_counts_59278rows.tsv``` 
  * Same as the above, but this file reports, for every image, a row containing the *count of instances* for each Detectron category.

