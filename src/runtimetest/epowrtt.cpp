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
        cout << "Input new block gap from now:" << endl;
        int gap;
        cin >> gap;
        CBlockIndex* pindexTip = chainActive.Tip();
        CBlockIndex* pindexRef = pindexTip->GetAncestor(pindexTip->nHeight - MINING_MARGIN_RATE);
        int64_t now = GetAdjustedTime();
        int64_t blockTime = now - gap;
        unsigned int tipTime = pindexTip->GetBlockTime();
        int64_t diffBlkTime = now - blockTime;
        int64_t blkDifftime = blockTime - tipTime;
        int64_t blkTermTime = (tipTime - pindexRef->GetBlockTime());
        cout << "now : " << now << endl;
        cout << "tip time : " << tipTime << endl;
        cout << "block time(pseudo) : " << blockTime << endl;
        cout << "blkDiffTime(block - tip) : " << blkDifftime << endl;
        cout << "diffBlkTime(now - block) : " << diffBlkTime << endl;
        cout << "blkTermTime(tip - ref block) : " << blkTermTime << endl;

        break;
    }

    return true;
}

void EpowDBRTT::displaySelfMenu()
{
     cout << "1. diffBlkTime" << endl;
}
