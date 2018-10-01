#ifndef BLOCKTREEDBRTT_H
#define BLOCKTREEDBRTT_H

#include "rttroot.h"

class BlockTreeDBRTT : public RTTRoot
{
public:
    const std::string title() const;

    bool runRunTimeTest(int index);

private:
    void displaySelfMenu();

    int chooseReadFlagMenu();

    void handleReadFlag(int readFlagIndex);
};

#endif // BLOCKTREEDBRTT_H
