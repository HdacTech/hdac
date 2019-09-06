// Copyright (c) 2014-2016 The Bitcoin Core developers
// Original code was distributed under the MIT software license.
// Copyright (c) 2014-2017 Coin Sciences Ltd
// MultiChain code distributed under the GPLv3 license, see COPYING file.

#ifndef BITCOIN_COINCONTROL_H
#define BITCOIN_COINCONTROL_H

#define FEATURE_HPAY_FUNDRAWTX

#include "primitives/transaction.h"

/** Coin Control Features. */
class CCoinControl
{
public:
    CTxDestination destChange;
#ifdef FEATURE_HPAY_FUNDRAWTX
    bool fAllowWatchOnly;
    bool fAllowOtherInputs;
    bool fOverrideFeeRate;
    CFeeRate nFeeRate;
    int fChangePosition;
#endif /* FEATURE_HPAY_FUNDRAWTX */
    CCoinControl()
    {
        SetNull();
    }

    void SetNull()
    {
        destChange = CNoDestination();
#ifdef FEATURE_HPAY_FUNDRAWTX
        fAllowWatchOnly = false;
        fOverrideFeeRate = false;
        nFeeRate = CFeeRate(0);
        fChangePosition = -1;
#endif /* FEATURE_HPAY_FUNDRAWTX */
        setSelected.clear();
    }

    bool HasSelected() const
    {
        return (setSelected.size() > 0);
    }

    bool IsSelected(const uint256& hash, unsigned int n) const
    {
        COutPoint outpt(hash, n);
        return (setSelected.count(outpt) > 0);
    }

    void Select(const COutPoint& output)
    {
        setSelected.insert(output);
    }

    void UnSelect(const COutPoint& output)
    {
        setSelected.erase(output);
    }

    void UnSelectAll()
    {
        setSelected.clear();
    }
#ifdef FEATURE_HPAY_FUNDRAWTX
    void ListSelected(std::vector<COutPoint>& vOutpoints) const
    {
        vOutpoints.assign(setSelected.begin(), setSelected.end());
    }
#else
    void ListSelected(std::vector<COutPoint>& vOutpoints)
    {
        vOutpoints.assign(setSelected.begin(), setSelected.end());
    }
#endif
private:
    std::set<COutPoint> setSelected;
};

#endif // BITCOIN_COINCONTROL_H
