#include "chainrtt.h"
#include <core/main.h>

using std::endl;
using std::cout;
using std::cin;
using std::vector;

const std::string ChainRTT::title() const
{
    return "Chain Runtime Test";
}

bool ChainRTT::runRunTimeTest(int index)
{
    switch (index) {
    case 1:
        cout << "chain height : " << chainActive.Height() << endl;
        std::vector<CBlockIndex*> vChain;
        cout << "size of CBlockIndex* : " << sizeof(CBlockIndex*) << endl;
        cout << "ref) vector max-size : " << vChain.max_size() << endl;
        cout << vChain.max_size()/sizeof(CBlockIndex*) - chainActive.Height()
             << " blocks may be inserted more : "
             << endl;
        break;
    }

    return true;
}

void ChainRTT::displaySelfMenu()
{
     cout << "1. height" << endl;
}
