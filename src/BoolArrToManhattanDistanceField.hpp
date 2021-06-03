#ifndef VOXELDEVSCRIPTS_DISTANCEFIELD_H
#define VOXELDEVSCRIPTS_DISTANCEFIELD_H

/*
 * The algorithm implemented in this file converts a flattened 3D bool array into a flattened 3D Manhattan distance field in linear time.
 * After executing all three passes (XPASS, YPASS, ZPASS) once, the conversion is completed.
 * Every entry in o_distanceField will contain the Manhattan distance to the closest corresponding "true" in the bool array (0 if it is true itself).
 *
 * The order XPASS -> YPASS -> ZPASS must be kept.
 * It is possible to add a "prepass" (initialization pass) that would make the order of X,Y,Z irrelevant, but doing so would increase the complexity by up to 33%.
 * It should be trivial to modify this implementation for other dimensions, non cubical arrays or for the ability to run it in parallel (with up to size^2) threads.
 *
 * The complexity of this algorithm is O(n) for n elements in boolArray, specifically: 6 * n
 * This algorithm iterates over the rows along each axis separately. It does so twice, once in each direction.
 */

// Note that size is the side length of the flattened 3D array, not the total count of it's members, which is size^3.
void boolArrToManhattanDFXPASS(const bool* boolArr, int* o_distanceField, int size)
{
    int size2 = size * size;

    for (int z = 0; z < size; z++)
    {
        for (int y = 0; y < size; y++)
        {
            // we first initialize the first distanceField element of the row with 0 or 2*size based on the bollArray's value
            o_distanceField[z * size2 + y * size + 0] = boolArr[z * size2 + y * size + 0] ? 0 : size + size;

            // we then iterate the row, setting each element to 0 or incrementing it by one over the previous entry
            for (int x = 1; x < size; x++)
                o_distanceField[z * size2 + y * size + x] = boolArr[z * size2 + y * size + x] ? 0 : 1 + o_distanceField[z * size2 + y * size + (x - 1)];

            // distance field values are now correct in increasing direction "behind" values that are true, but incorrect "before" them.
            // we iterate in opposite direction to adjust the distance values "before" values that are true.
            for (int x = size - 2; x >= 0; x--)
                if (o_distanceField[z * size2 + y * size + (x + 1)] < o_distanceField[z * size2 + y * size + x])
                    o_distanceField[z * size2 + y * size + x] = 1 + o_distanceField[z * size2 + y * size + (x + 1)];
        }
    }
}

// Note that size is the side length of the flattened 3D array, not the total count of it's members, which is size^3.
void boolArrToManhattanDFYPASS(int* o_distanceField, int size)
{
    int size2 = size * size;

    for (int z = start; z < size; z++)
    {
        for (int x = 0; x < size; x++)
        {
            // the field has already be initialized in XPASS, so we can now just adjust values that are occluded in "column" direction
            for (int y = 1; y < size; y++)
                if (o_distanceField[z * size2 + (y - 1) * size + x] < o_distanceField[z * size2 + y * size + x])
                    o_distanceField[z * size2 + y * size + x] = 1 + o_distanceField[z * size2 + (y - 1) * size + x];

            // and we repeat it the same way as before in opposite direction
            for (int y = size - 2; y >= 0; y--)
                if (o_distanceField[z * size2 + x + (y + 1) * size] < o_distanceField[z * size2 + x + y * size])
                    o_distanceField[z * size2 + y * size + x] = 1 + o_distanceField[z * size2 + (y + 1) * size + x];
        }
    }
}

// Note that size is the side length of the flattened 3D array, not the total count of it's members, which is size^3.
void boolArrToManhattanDFZPASS(int* o_distanceField, int size)
{
    int size2 = size * size;
    int maxDist = 2 * (size - 1);

    for (int y = 0; y < size; y++)
        for (int x = 0; x < size; x++)
        {
            // we adjust values that are occluded in "Z-column" direction
            for (int z = 1; z < size; z++)
                if (o_distanceField[(z - 1) * size2 + y * size + x] < o_distanceField[z * size2 + y * size + x])
                    o_distanceField[z * size2 + y * size + x] = 1 + o_distanceField[(z - 1) * size2 + y * size + x];

            // and we once again repeat it in opposite direction
            for (int z = size - 2; z >= 0; z--)
            {
                if (o_distanceField[(z + 1) * size2 + y * size + x] < o_distanceField[z * size2 + y * size + x])
                    o_distanceField[z * size2 + y * size + x] = 1 + o_distanceField[(z + 1) * size2 + y * size + x];
                // since this is the final pass we now also correct values that are out of bounds, this only happens if all entries in the bool array are 'false'
                o_distanceField[z * size2 + y * size + x] = o_distanceField[z * size2 + y * size + x] > maxDist ? maxDist : o_distanceField[z * size2 + y * size + x];
            }
            o_distanceField[(size - 1) * size2 + y * size + x] = o_distanceField[(size - 1) * size2 + y * size + x] > maxDist ? maxDist : o_distanceField[(size - 1) * size2 + y * size + x];
        }
}

// Since the order is fixed, there's really no point in having 3 separate functions
void boolArrToManhattanDF(const bool* boolArr, int* o_distanceField, int size)
{
    // note that the order is important and that only the first pass requires the bool array
    boolArrToManhattanDFXPASS(boolArr, distanceField, size);
    boolArrToManhattanDFYPASS(distanceField, size);
    boolArrToManhattanDFZPASS(distanceField, size);
}

// here is an example of how to use it
void main()
{
    int size = 64;

    bool* boolArr = new bool[size * size * size];
    memset(&boolArr, 0, size*size*size*sizeof(bool));

    // TODO: fill the bool array with (meaningful) data ...

    // note that we don't need to initialize the distance field, as our XPASS function overwrites any old data
    int* distanceField = new int[size * size * size];

    boolArrToManhattanDF(boolArr, distanceField, size);

    // TODO: do something with your distance field :D

    delete[] boolArr;
    delete[] distanceField;
}

#endif //VOXELDEVSCRIPTS_DISTANCEFIELD_H
