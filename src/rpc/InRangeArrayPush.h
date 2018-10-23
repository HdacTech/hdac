// Copyright (c) 2018 Hdac Technology AG
// Hdac code distributed under the GPLv3 license, see COPYING file.

#ifndef __INRANGEARRAYPUSH_H_
#define __INRANGEARRAYPUSH_H_

#include "json/json_spirit_utils.h"
#include "json/json_spirit_value.h"

class InRangeArrayPush
{
public:
    InRangeArrayPush(json_spirit::Array &array, int from, int count);

    void operator()(const std::string &item);

    void operator()(const json_spirit::Object &item);

private:
    json_spirit::Array &mArray;
    int mCurrentIndex;
    int mFrom;
    int mTo;
    bool mNotFilter;
};

#endif  /* __INRANGEARRAYPUSH_H_ */

