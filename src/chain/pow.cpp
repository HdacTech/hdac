// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Original code was distributed under the MIT/X11 software license.
// Copyright (c) 2014-2017 Coin Sciences Ltd
// MultiChain code distributed under the GPLv3 license, see COPYING file.
// Copyright (c) 2017 Hdac Technology AG
// Hdac code distributed under the GPLv3 license, see COPYING file.

#include "chain/pow.h"

#include "chain/chain.h"
#include "chainparams/chainparams.h"
#include "primitives/block.h"
#include "structs/uint256.h"
#include "utils/util.h"


#define SDA_BLOCK_SIZE        6      //   6 blocks
#define SDA_MAX_TIME_SPAN     10*60  // 600 sec
#define SDA_MIN_TIME_SPAN     1*60   //  60 sec
#define TIME_XFACTOR          8
#define TIME_FACTOR           4

#define LDA_TARGET_INTERVAL(x)   ( Params().Interval()/x )

bool IsEnabledDiffAlg(int nheight) 
{ 
    return ( nheight >= Params().GetStartHeightDiffAlg() ); 
}

static double GetDifficulty(unsigned int nBits)
{
    int nShift = (nBits >> 24) & 0xff;

    double dDiff =
        (double)0x0000ffff / (double)(nBits & 0x00ffffff);

    while (nShift < 29)
    {
        dDiff *= 256.0;
        nShift++;
    }
    while (nShift > 29)
    {
        dDiff /= 256.0;
        nShift--;
    }

    return dDiff;
}


static int64_t GetSDAadjustedTime(const CBlockIndex *pindexPrev, int64_t ldaActTime, int xfactor)
{
    int64_t result = 0;
    const int64_t ntimespan = SDA_MAX_TIME_SPAN;
    const int nPastBlockSize = SDA_BLOCK_SIZE;
    const int range = LDA_TARGET_INTERVAL(xfactor);
    
    if(pindexPrev->nHeight < nPastBlockSize*4)
    {
        return 0;
    }

    const CBlockIndex *pindexFirst = 
                     pindexPrev->GetAncestor(pindexPrev->nHeight - nPastBlockSize);

    int64_t nActualTimespan = pindexPrev->GetBlockTime() - pindexFirst->GetBlockTime();
    int64_t nAvgTime = nActualTimespan/nPastBlockSize;

    if(nAvgTime < ntimespan)
    {
        return 0;
    }
    if(ldaActTime/range >= nAvgTime)
    {
        return 0;
    }

    if(fDebug>1)LogPrintf("SDA avg-time (%d)\n", nAvgTime);

    if(nAvgTime > ntimespan*6)
    {
        result = ldaActTime>>1;
    }
    else if(nAvgTime > ntimespan*3)
    {
        result = ldaActTime>>2;
    }
    else
    {
        result = ldaActTime>>3;
    }

    if(fDebug>1)LogPrintf("adjusted time (%d)\n", result);
    
    return result;
}


static uint256 GetTargetPow(const CBlockIndex *pindexFirst,
                                   const CBlockIndex *pindexPrev,
                                   int xfactor, int factor) {
    assert(pindexPrev->nHeight > pindexFirst->nHeight);

    uint256 work = pindexPrev->nChainWork - pindexFirst->nChainWork;
    work *= Params().TargetSpacing();

    int64_t interval = LDA_TARGET_INTERVAL(xfactor);
    int64_t adjusttime = 0;
    
    int64_t nActualTimespan =
        int64_t(pindexPrev->nTime) - int64_t(pindexFirst->nTime);

    if(fDebug>1)LogPrintf("LDA avg-time (%d)\n", nActualTimespan/interval);

    adjusttime = GetSDAadjustedTime(pindexPrev, nActualTimespan, xfactor);
    nActualTimespan += adjusttime;
    
    if(adjusttime > 0)
        if(fDebug>1)LogPrintf("LDA + SDA time (%d)\n", nActualTimespan);

    int sda_factor = (adjusttime > 0) ? 2:1;
    
    if (nActualTimespan > (interval*factor*sda_factor) * Params().TargetSpacing()) {
    
        #if 1
        int lfactor = (interval*factor) * Params().TargetSpacing();
        if(sda_factor == 1)
        {
            if(nActualTimespan > (lfactor + (lfactor >> 1) ))
            {
                nActualTimespan = lfactor + (lfactor >> 1);
            }
            else if(nActualTimespan > (lfactor + (lfactor >> 2) ))
            {
                nActualTimespan = lfactor + (lfactor >> 2);
            }
            else
            {
                nActualTimespan = lfactor;
            }
        }
        else
        {
            nActualTimespan = (interval*factor*sda_factor) * Params().TargetSpacing();
        }
        #else
        nActualTimespan = (interval*factor*sda_factor) * Params().TargetSpacing();
        #endif

        if(fDebug>1)LogPrintf("adjustment over range %d\n", nActualTimespan);
        
    } else if (nActualTimespan < (interval/factor) * Params().TargetSpacing()) {
        if(fDebug>1)LogPrintf("adjustment lower range %d\n", nActualTimespan);
        nActualTimespan = (interval/factor) * Params().TargetSpacing();
    }
    
    work /= nActualTimespan;	

    return (-work) / work;
}

unsigned int ReduceWorkTarget(const CBlockIndex* pindexPrev, int64_t scale) {
    uint256 target;
    target.SetCompact(pindexPrev->nBits);
    target *= scale;

    uint256 powLimit = Params().ProofOfWorkLimit();
    if (target > powLimit) {
        target = Params().ProofOfWorkLimit();
    }

    if(fDebug>1)LogPrintf("Reduce Difficulty: (%d)\n", GetDifficulty(target.GetCompact()));
    
    return target.GetCompact();
}


/**
 * Long range Difficulty Adjustment
 * The entire range of the target is LDA_TARGET_INTERVAL(x).
 * LDA_TARGET_INTERVAL(8) --> 60 blocks
 */
unsigned int GetLDAWorkRequired(const CBlockIndex *pindexPrev,
                                 const CBlockHeader *pblock) {

    const int xfactor = TIME_XFACTOR;
    const int factor = TIME_FACTOR;
    
    assert(pindexPrev);

    const unsigned int nHeight = pindexPrev->nHeight;
    assert(nHeight >= LDA_TARGET_INTERVAL(xfactor));

    unsigned int nHeightFirst = nHeight - LDA_TARGET_INTERVAL(xfactor);
    const CBlockIndex *pindexFirst =
                           pindexPrev->GetAncestor(nHeightFirst);
    assert(pindexFirst);

    uint256 nextTarget =
        GetTargetPow(pindexFirst, pindexPrev, xfactor, factor);

    uint256 powLimit = Params().ProofOfWorkLimit();
    if (nextTarget > powLimit) {
        nextTarget = Params().ProofOfWorkLimit();
    }

    do
    {
        if(pindexPrev->nHeight < (Params().GetStartHeightNewHashAlog() - 1))
        {
            break;
        }
        
        if(pindexPrev->nHeight == (Params().GetStartHeightNewHashAlog() - 1))
        {
            return ReduceWorkTarget(pindexPrev, 100);
        }
        else if(pindexPrev->nHeight < (Params().GetStartHeightNewHashAlog() + LDA_TARGET_INTERVAL(TIME_XFACTOR)))
        {
            if(fDebug>1)LogPrintf("keep previous Difficulty( %d )\n", GetDifficulty(pindexPrev->nBits));
            return pindexPrev->nBits;
        }
		
        if(pindexPrev->nHeight < (Params().GetStartHeightBlockRewardAdj2nd() - 1))
        {
            break;
        }
        
        if(pindexPrev->nHeight == (Params().GetStartHeightBlockRewardAdj2nd() - 1))
        {
            return ReduceWorkTarget(pindexPrev, 10);
        }
        else if(pindexPrev->nHeight < (Params().GetStartHeightBlockRewardAdj2nd() + LDA_TARGET_INTERVAL(TIME_XFACTOR)))
        {
            if(fDebug>1)LogPrintf("keep previous Difficulty( %d )\n", GetDifficulty(pindexPrev->nBits));
            return pindexPrev->nBits;
        }
    }while(0);
	
        
    if(fDebug==1)LogPrintf("Set Difficulty: (%d)\n", GetDifficulty(nextTarget.GetCompact()));
	if(fDebug>1)LogPrintf("--------------------------------------------------------------------\n");
    if(fDebug>1)LogPrintf("LDA RETARGET\n");
    if(fDebug>1)LogPrintf("Before diff: %d\n", GetDifficulty(pindexPrev->nBits));
    if(fDebug>1)LogPrintf("After  diff: %d\n", GetDifficulty(nextTarget.GetCompact()));
	if(fDebug>1)LogPrintf("--------------------------------------------------------------------\n\n");

    return nextTarget.GetCompact();

}


/**
 * Short range Difficulty Adjustment
 * SDA_BLOCK_SIZE is last n blocks (6 ~ )
 * If producing the last n blocks took more than SDA_MAX_TIME_SPAN (10min ~),
 * reduces the difficulty.
**/
unsigned int GetSDAWorkRequired(const CBlockIndex* pindexPrev, const CBlockHeader *pblock)
{
    const int nEstimateBlockSize = SDA_BLOCK_SIZE;
    const int nMaxInterval = SDA_MAX_TIME_SPAN;
    unsigned int nextTarget;
    bool applySda = false;
    
    assert(pindexPrev);
    
    const CBlockIndex *pindexFirst =
                     pindexPrev->GetAncestor(pindexPrev->nHeight - nEstimateBlockSize);
    assert(pindexFirst);

    int64_t nEstTargetSpacing = nEstimateBlockSize * Params().TargetSpacing();
    int64_t nActualTimespan = pindexPrev->GetBlockTime() - pindexFirst->GetBlockTime();
    int64_t nAvgTime = nActualTimespan/nEstimateBlockSize;
    int64_t nTargetActTS;

    if (nAvgTime < nMaxInterval) {
        if(fDebug>1)LogPrintf("SDA skip ==> %d(%d-%d) blocks avg-time(%d)\n", 
                                                                 nEstimateBlockSize,
                                                                 pindexPrev->nHeight, 
                                                                 pindexFirst->nHeight, 
                                                                 nAvgTime);
        return pindexPrev->nBits;
    }
    

    nTargetActTS = nActualTimespan - (nActualTimespan>>2);    

    uint256 nPow;
    nPow.SetCompact(pindexPrev->nBits);
    
    nPow *= nTargetActTS;
    nPow /= nEstTargetSpacing;

    if(fDebug>1)LogPrintf("SDA enter ==> %d(%d-%d) blocks avg-time(%d)\n", 
                                                                 nEstimateBlockSize,
                                                                 pindexPrev->nHeight, 
                                                                 pindexFirst->nHeight, 
                                                                 nAvgTime);
    // Make sure we do not go below allowed values.
    if (nPow > Params().ProofOfWorkLimit())
        nPow = Params().ProofOfWorkLimit();

    if(fDebug>1)LogPrintf("--------------------------------------------------------------------\n");
    if(fDebug>1)LogPrintf("SDA RETARGET\n");
    if(fDebug>1)LogPrintf("Before diff: %d\n", GetDifficulty(pindexPrev->nBits));
    if(fDebug>1)LogPrintf("After  diff: %d\n", GetDifficulty(nPow.GetCompact()));
    if(fDebug>1)LogPrintf("--------------------------------------------------------------------\n\n");

    return nPow.GetCompact();
}


unsigned int GetTargetWorkRequired(const CBlockIndex* pindexPrev, const CBlockHeader *pblock)
{
    unsigned int nProofOfWorkLimit = Params().ProofOfWorkLimit().GetCompact();

    // Genesis block
    if (pindexPrev == NULL)
        return nProofOfWorkLimit;
    
    if(Params().Interval() <= 0)
    {
        return nProofOfWorkLimit;
    }
    
	// Special difficulty rule for testnet:
	// If the new block's timestamp is more than 2 * 3 minutes
	// then allow mining of a min-difficulty block.
    if (Params().AllowMinDifficultyBlocks() &&
        (pblock->GetBlockTime() >
         pindexPrev->GetBlockTime() + 2 * Params().TargetSpacing())) {
        return Params().ProofOfWorkLimit().GetCompact();
    }

    if(1)
    {
        return GetLDAWorkRequired(pindexPrev, pblock);
    }
    else
    {
        unsigned int nBits = GetSDAWorkRequired(pindexPrev, pblock);
        return (nBits == pindexPrev->nBits) ? GetLDAWorkRequired(pindexPrev, pblock) : nBits;
    }
}

unsigned int GetBTCWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock)
{
    unsigned int nProofOfWorkLimit = Params().ProofOfWorkLimit().GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;
    
    if(Params().Interval() <= 0)
    {
        return nProofOfWorkLimit;
    }
    // Only change once per interval
    if ((pindexLast->nHeight+1) % Params().Interval() != 0)
    {
        if (Params().AllowMinDifficultyBlocks())
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + Params().TargetSpacing()*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % Params().Interval() != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
        return pindexLast->nBits;
    }

    // Go back by what we want to be 14 days worth of blocks
    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < Params().Interval()-1; i++)
        pindexFirst = pindexFirst->pprev;
    assert(pindexFirst);

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();
    if(fDebug>1)LogPrintf("  nActualTimespan = %d  before bounds\n", nActualTimespan);
    if (nActualTimespan < Params().TargetTimespan()/4)
        nActualTimespan = Params().TargetTimespan()/4;
    if (nActualTimespan > Params().TargetTimespan()*4)
        nActualTimespan = Params().TargetTimespan()*4;

    // Retarget
    uint256 bnNew;
    uint256 bnOld;
    bnNew.SetCompact(pindexLast->nBits);
    bnOld = bnNew;
    bnNew *= nActualTimespan;
    bnNew /= Params().TargetTimespan();

    if (bnNew > Params().ProofOfWorkLimit())
        bnNew = Params().ProofOfWorkLimit();

    /// debug print
    if(fDebug>1)LogPrintf("GetBTCWorkRequired RETARGET\n");
    if(fDebug>1)LogPrintf("Params().TargetTimespan() = %d    nActualTimespan = %d\n", Params().TargetTimespan(), nActualTimespan);
    if(fDebug>1)LogPrintf("Before: %08x  %s\n", pindexLast->nBits, bnOld.ToString());
    if(fDebug>1)LogPrintf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString());

    return bnNew.GetCompact();
}


unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock)
{
    bool isEnabledNewAlg = false;
    
    do
    {
        const unsigned int nHeight = pindexLast->nHeight + 1;
        
        if(nHeight < Params().Interval())
        {
            isEnabledNewAlg = false;
            break;
        }
        if(IsEnabledDiffAlg(nHeight))
        {
            isEnabledNewAlg = true;
            break;
        }
    }
    while(0);

	if(isEnabledNewAlg)
	{
	    return GetTargetWorkRequired(pindexLast, pblock);
	}
	else
	{
	    return GetBTCWorkRequired(pindexLast, pblock);
	}

    
}


bool CheckProofOfWork(uint256 hash, unsigned int nBits,bool fNoErrorInLog)
{
    bool fNegative;
    bool fOverflow;
    uint256 bnTarget;

    if (Params().SkipProofOfWorkCheck())
       return true;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > Params().ProofOfWorkLimit())
        return error("CheckProofOfWork() : nBits below minimum work");

    // Check proof of work matches claimed amount    
    if (hash > bnTarget)
    {
        if(fNoErrorInLog)
        {
            return false;
        }
        else
        {
            return error("CheckProofOfWork() : hash doesn't match nBits");            
        }
    }

    return true;
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, unsigned int nVersion, unsigned int nHeight, bool fNoErrorInLog)
{
    if (Params().SkipProofOfWorkCheck())
       return true;

    if(nHeight >= Params().GetStartHeightNewHashAlog())
    {
        if(nVersion < 4)
        {
            
            return error("CheckProofOfWork() : mismatched hash algorithm");
        }
    }

    return CheckProofOfWork(hash, nBits, fNoErrorInLog);

    
}

uint256 GetBlockProof(const CBlockIndex& block)
{
    uint256 bnTarget;
    bool fNegative;
    bool fOverflow;
    bnTarget.SetCompact(block.nBits, &fNegative, &fOverflow);
    if (fNegative || fOverflow || bnTarget == 0)
        return 0;
    // We need to compute 2**256 / (bnTarget+1), but we can't represent 2**256
    // as it's too large for a uint256. However, as 2**256 is at least as large
    // as bnTarget+1, it is equal to ((2**256 - bnTarget - 1) / (bnTarget+1)) + 1,
    // or ~bnTarget / (nTarget+1) + 1.
    return (~bnTarget / (bnTarget + 1)) + 1;
}

