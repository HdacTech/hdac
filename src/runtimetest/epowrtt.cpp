#include "epowrtt.h"
#include <utils/timedata.h>
#include <core/main.h>

using std::endl;
using std::cout;
using std::cin;

const std::string EpowDBRTT::title() const
{
    return "EPOW Runtime Test";
}

bool EpowDBRTT::runRunTimeTest(int index)
{
    cout << "You select : " << index << ", " << __FUNCTION__ << endl;
    cout << endl;

    switch (index) {
    case 1:
        const int MINING_MARGIN_RATE=3;
        cout << "Input new block gap from tip:" << endl;
        int gap;
        cin >> gap;
        CBlockIndex* pindexTip = chainActive.Tip();
        CBlockIndex* pindexRef = pindexTip->GetAncestor(pindexTip->nHeight - MINING_MARGIN_RATE);
        int64_t adjustedTime = GetAdjustedTime();
        int64_t blockTime = adjustedTime + gap;
        unsigned int tipTime = pindexTip->GetBlockTime();
        int64_t diffBlkTime = adjustedTime - blockTime;
        int64_t blkDifftime = blockTime - tipTime;
        cout << "adjustedTime : " << adjustedTime << endl;
        cout << "tip block time(pseudo) : " << blockTime << endl;
        cout << "diffBlkTime : " << diffBlkTime << endl;
        cout << "blkDiffTime : " << blkDifftime << endl;
        break;
    }

    return true;
}

void EpowDBRTT::displaySelfMenu()
{
     cout << "1. diffBlkTime" << endl;
}
