# VoxelDevScripts
A collection of small functions/algorithms particularly useful for voxel game dev. Code will be in C and will be kept minimalistic and well commented so you can either copy&paste or implement/adopt it yourself. 

Because they are simple functions - meant to be copy&pasted - and not libraries, they will reside in an appropriately named header file. A basic usage example is given at the bottom.

If you find any of this useful, feel free to copy&paste, there's no need to credit or anything like that. 

If you find any bugs, please let me know so I can fix them :)


Scripts currently in this Repo:

File|Description
----|-----------
[BoolArrToManhattan](src/BoolArrToManhattan.h)|Converts a bool array to a [Manhattan](https://en.wikipedia.org/wiki/Taxicab_geometry) distance field in linear time. This may be useful for ray tracing, collision checking, or for building a flow field.
[Generic Vector Math](src/cpmath.h)|Single header vector math lib, offering vector types (2-4 components, float, int, uint), similar to glsl and basic arithmetic on these types. Uses C11 generics for nice syntax and SEE intrinsics for vec3 and vec4 types. For a more detailed description read the comment at the top of the file. I use this for my own game, but can not quarantee that it's suited for your application. Matrix math is not included in this yet but might be added in the future.
