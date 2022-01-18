#include "TopkQuery.h"

TopkQuery::TopkQuery(float xdim, float ydim, vector<uint64_t> k)
{
    x = xdim;
    y = ydim;
    keys = k;
}
