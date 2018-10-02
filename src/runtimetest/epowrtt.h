#ifndef EPOWRTT_H
#define EPOWRTT_H

#include "rttroot.h"

class EpowDBRTT : public RTTRoot
{
public:
    const std::string title() const;

    bool runRunTimeTest(int index);

private:
    void displaySelfMenu();
};

#endif // EPOWRTT_H
