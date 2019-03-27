import os
import cv2
import numpy as np
import PIL.Image
import IPython.display
import shutil
from utils.color_conversion import to_single_rgb, to_single_gray, rb_swap

try: # Python 2
    from cStringIO import StringIO as BytesIO
except: # Python 3
    from io import BytesIO
    
def imshow(img, fmt='png', retina=False, zoom=None):
    if img is None:
        raise TypeError('input image not provided')
    
    if len(img.shape) == 1:
        n = len(img)
        side = int(np.sqrt(n))
        if (side * side) == n:
            img = img.reshape(side, side)
        else:
            raise ValueError('input is one-dimensional', img.shape)
    if len(img.shape) == 3 and img.shape[-1] == 1:
        img = img.squeeze()
    img = np.uint8(np.clip(img, 0, 255))
    if fmt == 'jpg':
        fmt = 'jpeg'
    if fmt == 'jpeg':
        img = to_single_rgb(img)
    image_data = BytesIO()
    PIL.Image.fromarray(img).save(image_data, fmt)
    height, width = img.shape[:2]
    if zoom is not None:
        width *= zoom
        height *= zoom
    IPython.display.display(IPython.display.Image(data=image_data.getvalue(),
                                                  width=width,
                                                  height=height,
                                                  retina=retina))

# jpeg4py is 2x as fast as opencv for jpegs, but more unstable
def imread(filename, mode=None, ext=None):
    if ext is None:
        _, ext = os.path.splitext(filename)
    ext = ext.lower()
    img = cv2.imread(filename, cv2.IMREAD_UNCHANGED)
    if img is not None:
        if len(img.shape) > 2:
            img = rb_swap(img)
    if img is not None:
        if mode is 'rgb':
            img = to_single_rgb(img)
        elif mode is 'gray':
            img = to_single_gray(img)
    return img

def imwrite(filename, img):
    if img is not None:
        if len(img.shape) > 2:
            img = img[...,::-1]
    return cv2.imwrite(filename, img)

def downsample(img, scale=None, output_wh=None, max_side=None, min_side=None, block_size=None):
    if max_side is not None:
        cur_max_side = max(img.shape[:2])
        scale = max_side / cur_max_side
    if min_side is not None:
        cur_min_side = min(img.shape[:2])
        scale = min_side / cur_min_side
    if scale is not None:
        output_wh = (int(np.round(img.shape[1]*scale)),
                     int(np.round(img.shape[0]*scale)))
    if block_size is not None:
        output_wh = (img.shape[1]//block_size, img.shape[0]//block_size)
    else:
        block_size = img.shape[1]//output_wh[0]
    if block_size > 1:
        img = cv2.blur(img, (block_size, block_size))
    return cv2.resize(img, output_wh, cv2.INTER_AREA)

def upsample(img, scale=None, output_wh=None, max_side=None, min_side=None):
    if max_side is not None:
        cur_max_side = max(img.shape[:2])
        scale = max_side / cur_max_side
    if min_side is not None:
        cur_min_side = min(img.shape[:2])
        scale = min_side / cur_min_side
    if output_wh is None:
        output_wh = (int(np.round(img.shape[1]*scale)),
                     int(np.round(img.shape[0]*scale)))
    return cv2.resize(img, output_wh, cv2.INTER_CUBIC)

def imresize(img, scale=None, output_wh=None, max_side=None, min_side=None):
    big = True
    if max_side is not None:
        cur_max_side = max(img.shape[:2])
        big = max_side > cur_max_side
    elif min_side is not None:
        cur_min_side = min(img.shape[:2])
        big = min_side > cur_min_side
    elif output_wh is not None:
        big = output_wh[0] > img.shape[1]
    elif scale is not None:
        big = scale > 1
    
    if big:
        return upsample(img, scale, output_wh, max_side, min_side)
    else:
        return downsample(img, scale, output_wh, max_side, min_side)
