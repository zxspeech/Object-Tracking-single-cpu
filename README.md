# Object-Tracking-single-cpu
This is the C++ code for object tracking with particle filtering algorithm with adaptive and hybrid techniques.
The filtering algorithm is able to track the objects without training.

1. /NickPFTracking_hybrid/ contains the C++ code for object tracking with hybrid resampling algorithm.
2. /NickPFTracking_adaptive/ contains the C++ code for object tracking with adaptive resampling algorithm. In adaptive algorithm, the adaptive ratio needs to be predefined. 
3. The features of the targets can be defined for object tracking.
<p align="center"><img src=https://github.com/Solarbird2017/Object-Tracking-single-cpu/blob/main/11.png alt="Comparison"></p>
<p align="center"><img src=https://github.com/Solarbird2017/Object-Tracking-single-cpu/blob/main/21.png alt="Comparison"></p>
<p align="center"><img src=https://github.com/Solarbird2017/Object-Tracking-single-cpu/blob/main/31.png alt="Comparison"></p>
<p align="center"><img src=https://github.com/Solarbird2017/Object-Tracking-single-cpu/blob/main/41.png alt="Comparison"></p>


## usage
The code was tested on Mac and linux platorms.
1. install OpenCV.
2. install OpenMPI
3. export TMPDIR=/tmp
4. mpicxx main.cpp `pkg-config opencv --libs` -w
5. mpirun -np 4 ./a.out 10  // 4: use 4 cores, a.out: executed file, 10: 10 particles per core.

## Tracking accuracy

The matalb code in ./LabelingMatlabProgram/LabelingGT.m allow you to manually label the targets from a piece of video and save the labeled position of target in a txt file. The C++ program in ./PFTracking_adaptive and ./PFTracking_hybrid can read the txt file and compare it with the estimation position to calculate the accuracy (RMSE error).



