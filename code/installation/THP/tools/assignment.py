from numba import jit
import numpy as np
# import matplotlib.pyplot as plt
from sklearn.decomposition import FastICA
import os
import cv2
from glob import glob

@jit(nopython=True)
def auction(lhs, rhs, eps_beg=0.1, eps_end=0.001, eps_factor=5, thresh=0.2, thresh_factor=5):
    index_dtype = np.int32
    cost_dtype = np.float32
    n = len(lhs)
    price = np.zeros(n, dtype=cost_dtype)
    ismall = -1000000000 # smallest possible cost
    eps = eps_beg
    thresh *= n   
    phase = 0
    while True:
        print('phase', phase)
        phase += 1
        object_assign = -np.ones(n, dtype=index_dtype)
        owner_queue = list(range(n))
        while True:
            next_queue = [0][1:] # numba hack: make typed empty list
            for cur_owner in owner_queue:
                best_object = 0
                best_value = ismall
                next_value = ismall
                for cur_object in range(n):
                    dx = lhs[cur_object,0] - rhs[cur_owner,0]
                    dy = lhs[cur_object,1] - rhs[cur_owner,1]
                    a = dx * dx + dy * dy
                    a = -a
                    value = a - price[cur_object]
                    if value > next_value:
                        if value <= best_value:
                            next_value = value
                        else:
                            next_value = best_value
                            best_value = value
                            best_object = cur_object
                if object_assign[best_object] > -1:
                    next_queue.append(object_assign[best_object])
                object_assign[best_object] = cur_owner
                gamma = (best_value - next_value) + eps
                price[best_object] += gamma
            owner_queue = next_queue
            remaining = len(next_queue)
            if eps == eps_end and remaining == 0:
                return object_assign
            if remaining < thresh:
                break
        eps /= eps_factor
        thresh /= thresh_factor
        if eps < eps_end:
            eps = eps_end
            thresh = 0

def histogram_equalize_1d(x):
    indices = np.argsort(x)
    x[indices] = np.linspace(x.min(), x.max(), len(x))
    
def histogram_equalize_2d(x):
    histogram_equalize_1d(x[:,0])
    histogram_equalize_1d(x[:,1])
    
def sort_quadrants(xy):
    n = len(xy)
    y_mid = xy[:,0].mean()
    x_mid = xy[:,1].mean()
    quadrant = (xy[:,0]>y_mid)*2 + (xy[:,1]>x_mid)*1
    indices = np.argsort(quadrant)
    xy[indices] = xy
    
# this won't really work, because the edges of each quadrant
# don't line up. for it to work, you'd have to blend each quadrant
# into the the displacement of the next scale up.
def organize(xy, levels):
    histogram_equalize_2d(xy)
    if levels == 0:
        return
    sort_quadrants(xy)
    chunk = len(xy) // 4
    levels -= 1
    organize(xy[0*chunk:1*chunk], levels)
    organize(xy[1*chunk:2*chunk], levels)
    organize(xy[2*chunk:3*chunk], levels)
    organize(xy[3*chunk:4*chunk], levels)

def ica_standardize(xy):
    ica = FastICA()
    std = ica.fit_transform(xy)
    return std / std.std(axis=0)

def npy2tsv(npy):
    s = ""
    for i in range(npy.shape[0]):
        s+=str((0.5-npy[i][0])*20)+"\t"+str((0.5-npy[i][1])*20)+"\n"
    return s


def do_it(fn):

    lhs = np.load(fn)
    # side = int(np.sqrt(len(lhs)))
    # n = side * side
    side1 = 277
    side2 = 214

    n = side1 * side2

    print(f'{lhs.shape} => {side1}x{side2} => {n}')

    lhs = lhs[:n]

    lhs = ica_standardize(lhs)
    lhs -= lhs.min(axis=0)
    lhs /= lhs.max(axis=0)
    lhs = lhs.astype(np.float32)

    # plt.figure(figsize=(4,4))
    # plt.scatter(lhs[:,0], lhs[:,1], lw=0, s=2)
    # plt.show()

    xv, yv = np.meshgrid(np.linspace(0, 1, side1), np.linspace(0, 1, side2))
    rhs = np.dstack((xv, yv)).reshape(-1, 2).astype(np.float32)

    lhs_eq = np.copy(lhs)
    organize(lhs_eq, 0)

    # plt.figure(figsize=(4,4))
    # plt.scatter(lhs_eq[:,0], lhs_eq[:,1], lw=0, s=2)
    # plt.show()

    lhs.dtype == rhs.dtype, lhs.dtype == np.float32

    # matching rhs to lhs can be faster for the default case, slower if the points are all spread out
    # depth: Wall time: 8min 6s
    assign = auction(rhs, lhs_eq)

    # matching lhs to rhs is slower (5-7min)
    # depth: Wall time: 4min 12s
    # saliency: Wall time: 3min 30s
    # openface: Wall time: 5min 39s
    assign = auction(lhs_eq, rhs)

    assign_original = np.copy(assign)
    assign_invert = np.zeros_like(assign)
    for i, j in enumerate(assign):
        assign_invert[j] = i

    return np.ascontiguousarray(rhs[assign]).astype(np.float32)

    

files = [x for x in glob("../bin/data/embeddings/*/*.npy") if ("assignment" not in x)]
for f in files:
    f = f.replace("\\","/") #windows bullshit
    print(f)
    _,_,_,_,folder,fname = f.split("/")
    npy = do_it(f)
    file_npy = "../bin/data/embeddings/assignment/"+folder+"_"+fname
    np.save(file_npy,npy)

    tsv = npy2tsv(npy)
    file_tsv = file_npy.replace(".npy",".tsv")
    open(file_tsv,'w').write(tsv)


