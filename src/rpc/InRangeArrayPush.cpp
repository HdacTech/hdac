// Copyright (c) 2018 Hdac Technology AG
// Hdac code distributed under the GPLv3 license, see COPYING file.

#include "rpc/InRangeArrayPush.h"

using namespace json_spirit;
using namespace std;

InRangeArrayPush::InRangeArrayPush(Array &array,int from = -1, int count = -1) :
    mArray(array), mFrom(from), mTo(from+count), mCurrentIndex(0), mNotFilter(false) {
    if (from < 0 || count < 0)    {
        mNotFilter = true;
    }
}

void InRangeArrayPush::operator()(const string &item)
{
    if (mNotFilter) {
        mArray.push_back(item);
        return;
    }
    if (mCurrentIndex >= mFrom && mCurrentIndex < mTo)    {
        mArray.push_back(item);
    }
    ++mCurrentIndex;
}

void InRangeArrayPush::operator()(const Object &item)
{
    if (mNotFilter) {
        mArray.push_back(item);
        return;
    }
    if (mCurrentIndex >= mFrom && mCurrentIndex < mTo)    {
        mArray.push_back(item);
    }
    ++mCurrentIndex;
}
