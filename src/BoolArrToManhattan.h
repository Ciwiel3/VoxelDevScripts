#ifndef VOXELDEVSCRIPTS_DISTANCEFIELD_H
#define VOXELDEVSCRIPTS_DISTANCEFIELD_H

#include <stdbool.h>
#include <string.h>
#include <stdint.h>

/*
 * The algorithm implemented in this file converts a flattened 3D bool array into a flattened 3D Manhattan distance field in linear time.
 * After executing all three passes (XPASS, YPASS, ZPASS) once, the conversion is completed.
 * Every entry in o_distanceField will contain the Manhattan distance to the closest corresponding "true" in the bool array (0 if it is true itself).
 *
 * The order XPASS -> YPASS -> ZPASS must be kept.
 * It is possible to add a "prepass" (initialization pass) that would make the order of X,Y,Z irrelevant, but doing so would increase the complexity by up to 33%.
 * It should be trivial to modify this implementation for other dimensions, non cubical arrays or for the ability to run it in parallel (with up to size^2) threads.
 *
 * The complexity of this algorithm is O(n) for n elements in boolArray.
 * This algorithm iterates over the rows along each axis separately. It does so twice, once in each direction.
 */

static inline int min(int d1, int d2)
{
    return d1 > d2 ? d2 : d1;
}

static void boolArrToManhattanDFXPASS(const bool* boolArr, uint8_t* o_distanceField, int sizeX, int sizeY, int sizeZ)
{
    const int maxDistance = min(254, sizeX + sizeY + sizeZ);

    for (int z = 0; z < sizeZ; z++)
    {
        for (int y = 0; y < sizeY; y++)
        {
            // we first initialize the first distanceField element of the row with 0 or 3*size based on the bollArray's value
            o_distanceField[z * sizeX * sizeY + y * sizeX + 0] = boolArr[z * sizeX * sizeY + y * sizeX + 0] ? 0 : maxDistance;

            // we then iterate the row, setting each element to 0 or incrementing it by one over the previous entry
            for (int x = 1; x < sizeX; x++)
                o_distanceField[z * sizeX * sizeY + y * sizeX + x] =
                        boolArr[z * sizeX * sizeY + y * sizeX + x] ? 0 : min(maxDistance, 1 + o_distanceField[z * sizeX * sizeY + y * sizeX + (x - 1)]);

            // distance field values are now correct in increasing direction "behind" values that are true, but incorrect "before" them.
            // we iterate in opposite direction to adjust the distance values "before" values that are true.
            for (int x = sizeX - 2; x >= 0; x--)
                if (o_distanceField[z * sizeX * sizeY + y * sizeX + (x + 1)] < o_distanceField[z * sizeX * sizeY + y * sizeX + x])
                    o_distanceField[z * sizeX * sizeY + y * sizeX + x] = 1 + o_distanceField[z * sizeX * sizeY + y * sizeX + (x + 1)];
        }
    }
}

// Note that size is the side length of the flattened 3D array, not the total count of it's members, which is size^3.
static void boolArrToManhattanDFYPASS(uint8_t* o_distanceField, int sizeX, int sizeY, int sizeZ)
{
    for (int z = 0; z < sizeZ; z++)
    {
        for (int x = 0; x < sizeX; x++)
        {
            // the field has already be initialized in XPASS, so we can now just adjust values that are occluded in "column" direction
            for (int y = 1; y < sizeY; y++)
                if (o_distanceField[z * sizeX * sizeY + (y - 1) * sizeX + x] < o_distanceField[z * sizeX * sizeY + y * sizeX + x])
                    o_distanceField[z * sizeX * sizeY + y * sizeX + x] = 1 + o_distanceField[z * sizeX * sizeY + (y - 1) * sizeX + x];

            // and we repeat it the same way as before in opposite direction
            for (int y = sizeY - 2; y >= 0; y--)
                if (o_distanceField[z * sizeX * sizeY + (y + 1) * sizeX + x] < o_distanceField[z * sizeX * sizeY + y * sizeX + x])
                    o_distanceField[z * sizeX * sizeY + y * sizeX + x] = 1 + o_distanceField[z * sizeX * sizeY + (y + 1) * sizeX + x];
        }
    }
}

// Note that size is the side length of the flattened 3D array, not the total count of it's members, which is size^3.
static void boolArrToManhattanDFZPASS(uint8_t* o_distanceField, int sizeX, int sizeY, int sizeZ)
{
    for (int y = 0; y < sizeY; y++)
        for (int x = 0; x < sizeX; x++)
        {
            // we adjust values that are occluded in "Z-column" direction
            for (int z = 1; z < sizeZ; z++)
                if (o_distanceField[(z - 1) * sizeX * sizeY + y * sizeX + x] < o_distanceField[z * sizeX * sizeY + y * sizeX + x])
                    o_distanceField[z * sizeX * sizeY + y * sizeX + x] = 1 + o_distanceField[(z - 1) * sizeX * sizeY + y * sizeX + x];

            // and we once again repeat it in opposite direction
            for (int z = sizeZ - 2; z >= 0; z--)
            {
                if (o_distanceField[(z + 1) * sizeX * sizeY + y * sizeX + x] < o_distanceField[z * sizeX * sizeY + y * sizeX + x])
                    o_distanceField[z * sizeX * sizeY + y * sizeX + x] = 1 + o_distanceField[(z + 1) * sizeX * sizeY + y * sizeX + x];
            }
        }
}

// Since the order is fixed, there's really no point in having 3 separate functions
static void boolArrToManhattanDF(const bool* boolArr, uint8_t* o_distanceField, int sizeX, int sizeY, int sizeZ)
{
    // note that the order is important and that only the first pass requires the bool array
    boolArrToManhattanDFXPASS(boolArr, o_distanceField, sizeX, sizeY, sizeZ);
    boolArrToManhattanDFYPASS(o_distanceField, sizeX, sizeY, sizeZ);
    boolArrToManhattanDFZPASS(o_distanceField, sizeX, sizeY, sizeZ);
}

// Usage Example
void testBoolArrToManhattan()
{
    const int SIZE = 64;

    bool boolArr[SIZE * SIZE * SIZE];
    memset(&boolArr, 0, SIZE * SIZE * SIZE * sizeof(bool));

    // TODO: fill the bool array with (meaningful) data ...

    // note that we don't need to initialize the distance field, as our XPASS function overwrites any old data
    uint8_t distanceField[SIZE * SIZE * SIZE];

    boolArrToManhattanDF(boolArr, distanceField, SIZE, SIZE, SIZE);

    // TODO: do something with the distance field :D

    return 0;
}

#endif //VOXELDEVSCRIPTS_DISTANCEFIELD_H