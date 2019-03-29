# Teenie Harris Project Notebooks

These notebooks are one of two ways that the data for this project is processed and manipulated. The other is via Processing sketchbooks (in the `sketchbooks/` folder).

The overall process is:

1. Prepare and clean data, including resizing images, generating missing images, or basic analysis.
2. Create embedding of features (using [UMAP](https://github.com/lmcinnes/umap))
3. Create grid from embedding.

## Prepare and clean data

[Create Canonical File Order](https://github.com/CreativeInquiry/TeenieHarrisProject/blob/master/notebooks/Create%20Canonical%20File%20Order.ipynb) The original file structure for the photos is `Box_Name/Photo_Number.png`. Not all the box names are numbers, and not all the photo numbers are only numbers. This code uses a natural sort function to define a canonical order across all the files.

[Convert csv to sorted npy](https://github.com/CreativeInquiry/TeenieHarrisProject/blob/master/notebooks/Convert%20csv%20to%20sorted%20npy.ipynb) Some of the raw data is stored as a `.csv` and `filenames.txt` pair, where the `.csv` contains features (like `inceptionv3` or `vgg` features) and the `filenames.txt` describes which files the features belong to. This notebook uses the canonical image order and sorts the features to follow the same order.

[Create Missing Detectron Contour Images](https://github.com/CreativeInquiry/TeenieHarrisProject/blob/master/notebooks/Create%20Missing%20Detectron%20Contour%20Images.ipynb) Some of the images in the `detectron` dataset were missing .json files. Here we make up for that by finding images without any Detectron json, and generating black contour images.

[Get Total People](https://github.com/CreativeInquiry/TeenieHarrisProject/blob/master/notebooks/Get%20Total%20People.ipynb) Uses the `people_in_images` data to generate metadata useful for supervision: the categorical total number of faces in each photo.

[Create Resized Images](https://github.com/CreativeInquiry/TeenieHarrisProject/blob/master/notebooks/Create%20Resized%20Images.ipynb) Is a very flexible notebook that can take a folder of images and generate a cropped, resized set of images, or a .npy file containing all the images.

[Create Cropped Faces](https://github.com/CreativeInquiry/TeenieHarrisProject/blob/master/notebooks/Create%20Cropped%20Faces.ipynb) Generates cropped face photos based on the OpenFace data and original images, along with some metadata linking them back to the original OpenFace data.

## Create embedding of features

[Create Embeddings](https://github.com/CreativeInquiry/TeenieHarrisProject/blob/master/notebooks/Create%20Embeddings.ipynb) ingests all the different data products and outputs embeddings using nine different combinations of UMAP parameters.

## Create grid from embedding

[Create Grid from Embedding](https://github.com/CreativeInquiry/TeenieHarrisProject/blob/master/notebooks/Create%20Grid%20from%20Embedding.ipynb) Snaps the point cloud embeddings to a grid. Somewhat experimental and can be fairly slow (10 minutes for 60k points).

## Other

[Create Mosaic](https://github.com/CreativeInquiry/TeenieHarrisProject/blob/master/notebooks/Create%20Mosaic.ipynb) Useful for generating a single plot of the nine different UMAP outputs.

[Convert npy to tsv](https://github.com/CreativeInquiry/TeenieHarrisProject/blob/master/notebooks/Convert%20npy%20to%20tsv.ipynb) Demo of converting `.npy` files to `.tsv` for use in Processing sketchbooks.

[Combine Depth Saliency and Detectron](https://github.com/CreativeInquiry/TeenieHarrisProject/blob/master/notebooks/Combine%20Depth%20Saliency%20and%20Detectron.ipynb) Using small images from the resizing notebook, this shows a single grid/mosaic of detectron contours, saliency, and depth overlaid on the original photos.