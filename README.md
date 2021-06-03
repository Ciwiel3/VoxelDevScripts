# VoxelDevScripts
A collection of small functions/algorithms particularly useful for voxel game dev. Code will usually be in C++ and will be kept minimalistic and well commented. So you can either copy&paste it or implement/adopt it yourself. 

Because they are usually just simple functions - meant to be copy&pasted - and not libraries, they will simply reside in an appropriately named header file.

If you find any of this useful, feel free to copy&paste, there's no need to credit or anything like that. 

If you find any bugs, please let me know so I can fix them :)


Scripts currently in this Repo:

File|Description
----|-----------
BoolArrToManhattanDistanceField|Converts a bool array to a [Manhattan](https://en.wikipedia.org/wiki/Taxicab_geometry) distance field in linear time. This may be useful for ray tracing, collision checking, or for building a flow field.
