#ifndef CHAINRTT_H
#define CHAINRTT_H


#include "rttroot.h"

class ChainRTT : public RTTRoot
{
public:
    const std::string title() const;

    bool runRunTimeTest(int index);

private:
    void displaySelfMenu();
};

#endif // CHAINRTT_H
