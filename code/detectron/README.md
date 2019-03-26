# Detectron

Detectron is Facebook AI Research's software system that implements state-of-the-art object detection algorithms, including [Mask R-CNN](https://arxiv.org/abs/1703.06870). It is written in Python and powered by the [Caffe2](https://github.com/caffe2/caffe2) deep learning framework. It is available here: [https://github.com/facebookresearch/Detectron](https://github.com/facebookresearch/Detectron)

# Usage


`cd ~` to home folder

```
sudo nvidia-docker run -v `pwd`:/studio_home -it suhangpro/detectron /bin/bash


python2 /studio_home/Documents/Detectron/analyze_directory.py \
    --cfg /studio_home/Documents/Detectron/configs/12_2017_baselines/e2e_mask_rcnn_R-101-FPN_2x.yaml \
    --wts https://s3-us-west-2.amazonaws.com/detectron/35861858/12_2017_baselines/e2e_mask_rcnn_R-101-FPN_2x.yaml.02_32_51.SgT4y1cO/output/train/coco_2014_train:coco_2014_valminusminival/generalized_rcnn/model_final.pkl \
    --image-ext png \
    --image-dir /studio_home/TEENIE/Teenie_tiny \
    --output-dir /studio_home/TEENIE/Teenie_tiny_out \
    --json-dir /studio_home/TEENIE/Teenie_tiny_json



'mask_rle'
'objects' -> list
 each object
 'box'
 'class'
 'mask_idx'
 'contours'
 'score'
```

# More notes

```
python2 analyze_directory.py \
    --cfg configs/12_2017_baselines/e2e_mask_rcnn_R-101-FPN_2x.yaml \
    --output-dir /home/studio/TEENIE/teenie_test \
    --image-ext jpg \
    --wts https://s3-us-west-2.amazonaws.com/detectron/35861858/12_2017_baselines/e2e_mask_rcnn_R-101-FPN_2x.yaml.02_32_51.SgT4y1cO/output/train/coco_2014_train:coco_2014_valminusminival/generalized_rcnn/model_final.pkl \
    /home/studio/TEENIE/Teenie_Harris_PNG1024/Box_019 

sudo nvidia-docker run -v `pwd`:/studio_home -it suhangpro/detectron /bin/bash
```

