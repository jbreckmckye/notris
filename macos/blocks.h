#include "defs.h"

typedef int shapeHex;
typedef int rotationIndex;

int getShapeBit(shapeHex s);
rotationIndex getNextRotation(int r);
shapeHex getBlockShape(BlockNames key, rotationIndex r);
BlockColours getBlockColour(BlockNames key);

BlockNames randomBlock();
