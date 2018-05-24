// Copyright (c) 2017 Hdac Technology AG
// Hdac code distributed under the GPLv3 license, see COPYING file.

/*
  Hdac ePoW is a consensus algorithm that reduces the mining oligopoly by applying the block window concept.
  Also, it is an algorithm to reduce the wasteful energy consumed in the hash calculation by avoiding spontaneous mining attempt
  during the block window application period after the mining success.
  If a node succeeds in mining, no new block can be mined during the block window application period.
  Even if a greedy node neglects this mechanism and mined a new block,
  it will not be recognized as a valid block in the entire network of Hdac blockchains,
  so there is no reason to try to find an invalid block.

  * ePoW consensus algorithm basic condition
 	1. Blocks corresponding to the block window are not accepted as valid blocks. (Full Node)
 	2. The block corresponding to the block window does not propagate. (Full Node)
	3. During the block window, no mining is done spontaneously. (Miner or Mining Pool)
*/

#include "chain/epow.h"

#include "structs/amount.h"
#include "primitives/block.h"
#include "primitives/transaction.h"
#include "structs/hash.h"
#include "core/main.h"
#include "net/net.h"
#include "structs/base58.h"
#include "chain/pow.h"
#include "utils/timedata.h"
#include "utils/util.h"
#include "utils/utilmoneystr.h"
#ifdef ENABLE_WALLET
#include "wallet/wallet.h"
#endif

#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>

using namespace std;

const int 	BLOCK_NUM_OF_MAX_BLOCKWINDOW = 0x001ABBC0;	// 1752000; <<-- TODO
//const int 		BLOCK_NUM_OF_MAX_BLOCKWINDOW = 10000; // 1752;	// <<-- TODO
const int 		MAX_MINING_DEPTH_OF_BLOCKWINDOW = 0x02BC;	// 700;	// 1000 * 0.7
const float 	BLOCKWINDOW_NODE_FACTOR_RATE = 0.7f;
const int 		BLOCKWINDOW_MINING_BLACKOUT_TIMESAPN = 0x021C;	// 60*9, 60*12 02D0
//const int 		HDAC_TARGET_BLOCK_TIME_MULTIPLE 		= 3;
const int 		HDAC_TARGET_BLOCK_TIME_MULTIPLE 		= 20; // HDAC  blackouttime change (9min --> 60min)
const int 		HDAC_TARGET_BLOCK_TIME_MULTIPLE_DEEP 	= 4;
const int		NODEFACTOR_DEFAULT_DEPTH = 0x01E0; // 480

bool	BLOCKWINDOW_TOUCHED = true;
int		BLOCKWINDOW_LAST_SZ = 0;

int		BW_CUR_WZ = 0;
int		BW_CUR_NF = 0;
int		BW_CUR_BH = 0;

typedef boost::unordered_map<std::string, int> NodeFactorList;

extern set <std::string> setBlacklistBlocks;		// HDAC

/*
  The block window size is calculated as a block window function with the mining node parameter as the main variable.
 * Calculation of mining node factor
    - The number of successful miners is the number of miners who succeeded in mining for a certain period of time.
    - If the number of mined blocks in a specific period is defined as 1000,
      That is, (current block height) - 1000.
    - In the last [1 days, 480 blocks], the number of nodes with mining history and the number of mining

 * The block number at the time of reaching the maximum block window
    - Block Time - 3 minutes
    - 24h * 60m / 3m = 480 pieces (one day)
    - 480 * 10 years = 1,752,000 (10 years)
    - BLOCK_NUM_OF_MAX_BLOCKWINDOW = 1,752,000
     (BLOCK_NUM_OF_MAX_BLOCKWINDOW: Block number of block window maximum size (10 years; 3 minutes; 1,752,000))
*/

// Verify the block window policy against the miner of the received block
bool VerifyBlockWindow(const CBlock& block, CNode* pfrom)
{
	//if(!MCP_ANYONE_CAN_MINE)
	//	return true;

	if(chainActive.Tip()!=NULL)
		BW_CUR_BH = chainActive.Tip()->nHeight;

	int64_t blockTime=block.GetBlockTime();
	int64_t diffBlkTime, now=GetAdjustedTime();
	diffBlkTime = now - blockTime;

	if(pfrom != NULL && diffBlkTime > (MCP_TARGET_BLOCK_TIME*HDAC_TARGET_BLOCK_TIME_MULTIPLE))	// CIH
	{
		if(fDebug)
		{
			std::string msg = strprintf("\t>> BLOCK TIME DIFF: %d BLK T: %d ADJ T: %d RCV FROM: %s\nBLOCK_TIME: %d(sec)", diffBlkTime, blockTime, now, (pfrom == NULL ? "ME":">>>"+pfrom->addr.ToString()), MCP_TARGET_BLOCK_TIME);
			LogPrintf("hdac: %s\n", msg);
			//std::cout << msg << std::endl;
		}

		return true;
	}

	std::string strMiner = GetCoinbaseAddress(block);
	if(strMiner.length()<0x20)
	{
		std::string msg = strprintf("%s Miner address is invalid. ADDR=%s", (pfrom == NULL ? "ME": ">>>"+pfrom->addr.ToString()), strMiner);
		LogPrintf("hdac: %s\n", msg);
		//if(fDebug)std::cout << std::endl << msg << std::endl;

		return false;
	}

	if(IsMiningBlackout())	// CIH ??????? - ponder more deeply
	{
		bool fRet = false;
		int miningContinuity = GetMiningContinuity(strMiner);
		int nf = GetNodeFactor(NODEFACTOR_DEFAULT_DEPTH);

		if(nf < 3) {//if(nf < 5) {				// <<-- TODO
			fRet = true;
		} else if(nf < 20)	{
			if(miningContinuity<2)
				fRet = true;
		} else if(nf < 40)	{
			if(miningContinuity<3)
				fRet = true;
		} else if(nf < 80)	{
			if(miningContinuity<4)
				fRet = true;
		} else{
			if(miningContinuity<5)
				fRet = true;
		}

		if(fRet)
		{
			std::string msg = strprintf("%s Mining Blackout mode was applied... MINER: %s CONTINUITY: %d", (pfrom == NULL ? "ME": ">>>"+pfrom->addr.ToString()), strMiner, miningContinuity);
			LogPrintf("hdac: %s\n", msg);
			//if(fDebug) std::cout << std::endl << msg << std::endl;

			return fRet;
		}
	}

	if(fDebug)
	{
		std::string msg = strprintf("%s BLOCK TIME DIFF: %d BLK T: %d ADJ T: %d  TARGET_BLOCK_TIME: %d(sec)", (pfrom == NULL ? "":">>>"+pfrom->addr.ToString()), diffBlkTime, blockTime, now, MCP_TARGET_BLOCK_TIME);
		LogPrintf("hdac: %s\n", msg);
		//std::cout << msg << std::endl;
	}

	return CheckBlockWindow(strMiner);
}


/*
   Mining Blackout Monitors whether the judgment condition is satisfied.
    - If a new block is not created even after more than 60 minutes (3 block times) after the last block
     (Changed from 9 minutes to 60 minutes - 2018/01/23)

    - Transition to Mining blackout mode
    - Check the operation of the basic mining node (if it is a basic mining node)
    - The mining node immediately switches from 'mining wait mode' to 'mining mode', even within the block window.
    - If is the Mining blackout mode - All Full Nodes accept new block of mining node in block window.
*/

bool IsMiningBlackout()
{
	bool ret=false;

	CBlockIndex* pindexTip = chainActive.Tip();
	if(pindexTip==NULL)
		return false;

	int64_t blockTime = pindexTip->GetBlockTime();
	int64_t diffBlkTime, now=GetAdjustedTime();
	diffBlkTime = now-blockTime;

	if(diffBlkTime > (MCP_TARGET_BLOCK_TIME*HDAC_TARGET_BLOCK_TIME_MULTIPLE))
	{
		int depth = NODEFACTOR_DEFAULT_DEPTH;	// 0x01E0;	// 480 blks per day

		int wz = GetBlockWindowSize();
		int nf = GetNodeFactor(depth);
		ret = (wz<nf ? true : false);

		std::string msg = strprintf("IsMiningBlackout? %s. DIFF TM: %d(sec) WZ: %d NF: %d", ret?"true":"false", diffBlkTime, wz, nf );
		LogPrintf("hdac: %s\n", msg);
		//if(fDebug) std::cout << std::endl << msg << std::endl;

		return ret;
	}

	return ret;
}


bool CheckBlockWindow(std::string strMiner)
{
	int wz = 0;
	int miningDepth = GetMiningDepth(strMiner);
	//nodeFactor = GetNodeFactor(NODEFACTOR_DEFAULT_DEPTH);
	wz = GetBlockWindowSize();

	if(miningDepth<0)
	{
		if(fDebug)
		{
			std::string msg = strprintf("WZ: %d MINER: %s M-DEPTH: %d", wz, strMiner, miningDepth);
			LogPrintf("hdac: %s\n", msg);
			//std::cout << msg << std::endl;
		}

		return true;
	}

	if(wz>miningDepth)
	{
		if(fDebug)
		{
			std::string msg = strprintf("CheckBlockWindow FALSE. WZ: %d > M-DEPTH: %d, MINER: %s", wz, miningDepth, strMiner);
			LogPrintf("hdac: %s\n", msg);
			//std::cout << msg << std::endl;
		}

		return false;
	}

	if(fDebug)
	{
		std::string msg = strprintf("WZ: %d MINER: %s M-DEPTH: %d", wz, strMiner, miningDepth);
		LogPrintf("hdac: %s\n", msg);
		//std::cout << msg << std::endl;
	}

	return true;
}

bool CheckBlockWindow(CPubKey minerPubKey)
{
	bool ret=true;

	CBitcoinAddress addr = CBitcoinAddress(minerPubKey.GetID());
	std::string strMiner = addr.ToString();

	return CheckBlockWindow(strMiner);
}

/*
 Block window function

1. If x is greater than 'the block number of the block window maximum size'
    1) Set x value to 'the block number of the block window maximum size'
    2) The block window size is (0.7f * node factor)

2. Otherwise
    1) The block window size is (0.7f * x * node factor)
     The block window divided by 'the block number of the block window maximum size'

3. If the block window size is greater than or equal to the 'node factor'
    1) Block window size is set to (node factor - 1)

- x: Last size of block window (BLOCKWINDOW_LAST_SZ)
    (if there is an activeChain, the height of the activeChain)
- node factor (the number of nodes in the last 480 blocks)
- block number (10 years; 3 minutes; 1,752,000)
*/
int GetBlockWindowSize()
{
	//if(!MCP_ANYONE_CAN_MINE)
	//		return 0;

	if(!BLOCKWINDOW_TOUCHED)
	{
		//std::string msg = strprintf("GetBlockWindowSize: blockwindow touched? %s, last_blockwindow_sz=%d", (g_blockwindow_touched ? "true":"false"), last_blockwindow_sz);
		//LogPrintf("hdac: %s\n", msg);
		//std::cout << msg << std::endl;

		return BLOCKWINDOW_LAST_SZ;
	}

	int blockWz = -1;	// Block Window Size

	// DEPTH*1
	//int nf = 0; // Node Factor
	int depth = NODEFACTOR_DEFAULT_DEPTH;	// 0x01E0;	// 480 blks per day
	int x = 1;
	int mxBlkWz = BLOCK_NUM_OF_MAX_BLOCKWINDOW;

	int nf = GetNodeFactor(depth);

	if(chainActive.Tip()!=NULL)
		x = chainActive.Tip()->nHeight;
	else
		return BLOCKWINDOW_LAST_SZ;

	BW_CUR_BH = x;

	// DEPTH*2
	/*
	int nf2 = 0; // Node Factor
	int depth2 = 0x03C0;	// 480*2 blks

	nf2 = GetNodeFactor(depth2);
	*/


	// consider N < 10
	if(x>BLOCK_NUM_OF_MAX_BLOCKWINDOW)
	{
		x = BLOCK_NUM_OF_MAX_BLOCKWINDOW;
		blockWz = (int) std::floor(BLOCKWINDOW_NODE_FACTOR_RATE*nf);
	}
	else
	{
		blockWz = (int) std::floor((BLOCKWINDOW_NODE_FACTOR_RATE*x*nf)/BLOCK_NUM_OF_MAX_BLOCKWINDOW);
	}

	// check once again
	if(blockWz >= nf)
		blockWz = nf-1;

	BLOCKWINDOW_LAST_SZ = blockWz;
	BLOCKWINDOW_TOUCHED = false;

	BW_CUR_WZ = blockWz;

	return blockWz;
}

int GetBlockWindowSize(int nStartDepth)  // HDAC
{
	int blockWz = -1;	// Block Window Size

	// DEPTH*1
	//int nf = 0; // Node Factor
	int depth = NODEFACTOR_DEFAULT_DEPTH;	// 0x01E0;	// 480 blks per day
	int x = 1;
	int mxBlkWz = BLOCK_NUM_OF_MAX_BLOCKWINDOW;

	int nf = GetNodeFactor(depth, nStartDepth);
	if (nf == -1)
		return -1; // error

	int i=0;
	CBlockIndex* pindex = NULL;
	pindex = chainActive.Tip();
	for (i=0;
		   i<depth && pindex != NULL && pindex->pprev && (i < nStartDepth);
		   pindex = pindex->pprev, i++)
		;

	if(pindex != NULL)
		x = pindex->nHeight;
	else
		return -1;  // error

	// DEPTH*2
	/*
	int nf2 = 0; // Node Factor
	int depth2 = 0x03C0;	// 480*2 blks

	nf2 = GetNodeFactor(depth2);
	*/


	// consider N < 10
	if(x>BLOCK_NUM_OF_MAX_BLOCKWINDOW)
	{
		x = BLOCK_NUM_OF_MAX_BLOCKWINDOW;
		blockWz = (int) std::floor(BLOCKWINDOW_NODE_FACTOR_RATE*nf);
	}
	else
	{
		blockWz = (int) std::floor((BLOCKWINDOW_NODE_FACTOR_RATE*x*nf)/BLOCK_NUM_OF_MAX_BLOCKWINDOW);
	}

	// check once again
	if(blockWz >= nf)
		blockWz = nf-1;

	return blockWz;
}

// node factor (the number of nodes in the last 480 blocks)
int GetNodeFactor(int depth)
{
    /* // CIH 2017.12.07
	if(!MCP_ANYONE_CAN_MINE)
	{
		BW_CUR_NF = mc_gState->m_Permissions->GetMinerCount();   // GetActiveMinerCount with diversity=0
		return BW_CUR_NF;
	}
	*/

	int nodeFactor=0;
	if(depth<1)
		depth = 0;

	int i=0;

	CBlockIndex* pindex = NULL;
	CBlockIndex* pindexTip = chainActive.Tip();
	int nHeight = chainActive.Height();
	int idx = nHeight-depth+1;
	if(nHeight<0 || idx<0)
		idx=0;

	if(pindexTip == NULL)
		return BW_CUR_NF;

	/*
	NodeFactorList list;
	std::cout << "TIP H: " << nHeight << std::endl;

	pindex = chainActive[idx];

	for (; pindex ; pindex = chainActive[++idx])
	{
		CBlock block;
		if(!ReadBlockFromDisk(block, pindex))
		{
			std::string msg = "Can't read block from disk";
			LogPrintf("hdac: %s\n", msg);
			std::cout << msg << std::endl;
			continue;
		}

		std::string str_addr_out;

		if(block.vtx.size()>0 && block.vtx[0].IsCoinBase())
		{
			const CTxOut& txout = block.vtx[0].vout[0];

			txnouttype whichType;
			const CScript& prevScript = txout.scriptPubKey;
			vector<CTxDestination> addressRets;
			int nRequiredRet;

			if(!ExtractDestinations(prevScript,whichType,addressRets,nRequiredRet))
			{
				std::string msg = "ExtractDestination() fail.";
				LogPrintf("hdac: %s\n", msg);
				std::cout << msg << std::endl;
			}
			else
			{
				if(addressRets.size()>0)
				{
					const CTxDestination &addr = addressRets[0];
					str_addr_out = CBitcoinAddress(addr).ToString();

					std::cout << "H: "<<pindex->nHeight <<" IDX: " << idx << " ADDR: " << str_addr_out << std::endl;

					list.insert(make_pair(str_addr_out, pindex->nHeight));
				}
			}
		}
	}

	std::cout << std::endl;
	std::cout << "list  :" << list.size() << std::endl;

	NodeFactorList::iterator it = list.begin();
	while(it != list.end())
	{
		std::cout << "" << it->first << " " << it->second << std::endl;

		it++;
	}
	std::cout << std::endl;
	*/

	NodeFactorList nodeFactorList;
	pindex = chainActive.Tip();
	for (i=0;
		   i<depth && pindex != NULL && pindex->pprev;
		   pindex = pindex->pprev, i++)
	{
		CBlock block;
		if(!ReadBlockFromDisk(block, pindex))
		{
			std::string msg = "Can't read block from disk";
			LogPrintf("hdac: %s\n", msg);
			//if(fDebug)std::cout << msg << std::endl;
			continue;
		}

		std::string str_addr_out;

		if(block.vtx.size()>0 && block.vtx[0].IsCoinBase())
		{
			const CTxOut& txout = block.vtx[0].vout[0];

			txnouttype whichType;
			const CScript& prevScript = txout.scriptPubKey;
			vector<CTxDestination> addressRets;
			int nRequiredRet;

			if(!ExtractDestinations(prevScript,whichType,addressRets,nRequiredRet))
			{
				std::string msg = "ExtractDestination() fail.";
				LogPrintf("hdac: %s\n", msg);
				//if(fDebug)std::cout << msg << std::endl;
			}
			else
			{
				if(addressRets.size()>0)
				{
					const CTxDestination &addr = addressRets[0];
					str_addr_out = CBitcoinAddress(addr).ToString();

					//std::cout << "H: "<<pindex->nHeight <<" IDX: " << idx << " ADDR: " << str_addr_out << std::endl;

					//nodeFactorList.insert(make_pair(str_addr_out, pindex->nHeight));
//			           if(setBlacklistBlocks.size())  // HDAC
//			            {
//			                if(setBlacklistBlocks.find(str_addr_out) == setBlacklistBlocks.end())
//			                {
//								nodeFactorList.insert(make_pair(str_addr_out, pindex->nHeight));
//			                }
//			            }
//			           else
//			           {

					nodeFactorList.insert(make_pair(str_addr_out, pindex->nHeight));
//			           }
				}
			}
		}

	}

	/*
	NodeFactorList::iterator it2 = list2.begin();
	std::cout << std::endl;
	std::cout << "list2 :" << list2.size() << std::endl;

	while(it2 != list2.end())
	{
		std::cout << "" << it2->first << " " << it2->second << std::endl;

		it2++;
	}
	std::cout << std::endl;
	*/

	nodeFactor = nodeFactorList.size();	//

	BW_CUR_NF = nodeFactor;

	return nodeFactor;
}

// GetNodeFactor for given height
int GetNodeFactor(int depth, int nStartDepth)  		// HDAC
{
 	int nodeFactor=0;
	if(depth<1)
		depth = 0;

	int i=0;

	CBlockIndex* pindex = NULL;
	//CBlockIndex* pindexTip = chainActive.Tip();
	pindex = chainActive.Tip();
	for (i=0;
		   i<depth && pindex != NULL && pindex->pprev && (i < nStartDepth);
		   pindex = pindex->pprev, i++)
		;

	if(pindex == NULL)
		return -1;  // error

	int nHeight = pindex->nHeight;
	int idx = nHeight-depth+1;
	if(nHeight<0 || idx<0)
		idx=0;

	NodeFactorList nodeFactorList;
	//pindex = chainActive.Tip();
	for (i=0;
		   i<depth && pindex != NULL && pindex->pprev;
		   pindex = pindex->pprev, i++)
	{
		CBlock block;
		if(!ReadBlockFromDisk(block, pindex))
		{
			std::string msg = "Can't read block from disk";
			LogPrintf("hdac: %s\n", msg);
			//if(fDebug)std::cout << msg << std::endl;
			continue;
		}

		std::string str_addr_out;

		if(block.vtx.size()>0 && block.vtx[0].IsCoinBase())
		{
			const CTxOut& txout = block.vtx[0].vout[0];

			txnouttype whichType;
			const CScript& prevScript = txout.scriptPubKey;
			vector<CTxDestination> addressRets;
			int nRequiredRet;

			if(!ExtractDestinations(prevScript,whichType,addressRets,nRequiredRet))
			{
				std::string msg = "ExtractDestination() fail.";
				LogPrintf("hdac: %s\n", msg);
				//if(fDebug)std::cout << msg << std::endl;
			}
			else
			{
				if(addressRets.size()>0)
				{
					const CTxDestination &addr = addressRets[0];
					str_addr_out = CBitcoinAddress(addr).ToString();

					nodeFactorList.insert(make_pair(str_addr_out, pindex->nHeight));
				}
			}
		}

	}

	nodeFactor = nodeFactorList.size();	//

	return nodeFactor;
}
void GetCurrentBlockWindowInfo(int& wz, int& nf, int& bh)
{
	wz = BW_CUR_WZ;
	nf = BW_CUR_NF;
	bh = BW_CUR_BH;
}

int GetMiningDepth(const std::string strAddr)
{
	int miningDepth = -1;
	int maxDepth = MAX_MINING_DEPTH_OF_BLOCKWINDOW;
	std::string str_addr_out;
	CBlockIndex* 	pindex = chainActive.Tip();

	for (int idx=0;
			idx<maxDepth && pindex != NULL && pindex->pprev;
		   pindex = pindex->pprev, idx++)
	{
		CBlock block;
		if(!ReadBlockFromDisk(block, pindex))
		{
			std::string msg = "Can't read block from disk";
			LogPrintf("hdac: %s\n", msg);
			//if(fDebug)std::cout << msg << std::endl;
			continue;
		}

		if(block.vtx.size()>0 && block.vtx[0].IsCoinBase())
		{
			const CTxOut& txout = block.vtx[0].vout[0];

			txnouttype whichType;
			const CScript& prevScript = txout.scriptPubKey;
			vector<CTxDestination> addressRets;
			int nRequiredRet;

			if(!ExtractDestinations(prevScript,whichType,addressRets,nRequiredRet))
			{
				std::string msg = "ExtractDestination() fail.";
				LogPrintf("hdac: %s\n", msg);
				//if(fDebug)std::cout << msg << std::endl;
			}
			else
			{
				if(addressRets.size()>0)
				{
					const CTxDestination &addr = addressRets[0];
					str_addr_out = CBitcoinAddress(addr).ToString();

					//std::cout << "IDX: " << idx << " H: " << pindex->nHeight << " BLK ADDR: " << str_addr_out << " <-" << (strAddr.compare(str_addr_out) ? "X":"O") << "-> MINER: " << strAddr << std::endl;

					if(strAddr.compare(str_addr_out) == 0)
					{
						miningDepth = idx;	// x idx+1
						break;
					}
				}
			}
		}

	}

	return miningDepth;
}

int GetMiningContinuity(const std::string strAddr)
{
	int miningContinuity = 0;
	int maxDepth = MAX_MINING_DEPTH_OF_BLOCKWINDOW;
	std::string str_addr_out;
	CBlockIndex* 	pindex = chainActive.Tip();

	for (int idx=0;
			idx<maxDepth && pindex != NULL && pindex->pprev;
		   pindex = pindex->pprev, idx++)
	{
		CBlock block;
		if(!ReadBlockFromDisk(block, pindex))
		{
			std::string msg = "Can't read block from disk";
			LogPrintf("hdac: %s\n", msg);
			//if(fDebug)std::cout << msg << std::endl;
			continue;
		}

		if(block.vtx.size()>0 && block.vtx[0].IsCoinBase())
		{
			const CTxOut& txout = block.vtx[0].vout[0];

			txnouttype whichType;
			const CScript& prevScript = txout.scriptPubKey;
			vector<CTxDestination> addressRets;
			int nRequiredRet;

			if(!ExtractDestinations(prevScript,whichType,addressRets,nRequiredRet))
			{
				std::string msg = "ExtractDestination() fail.";
				LogPrintf("hdac: %s\n", msg);
				//if(fDebug)std::cout << msg << std::endl;
			}
			else
			{
				if(addressRets.size()>0)
				{
					const CTxDestination &addr = addressRets[0];
					str_addr_out = CBitcoinAddress(addr).ToString();

					//std::cout << "IDX: " << idx << " H: " << pindex->nHeight << " BLK ADDR: " << str_addr_out << " <-" << (strAddr.compare(str_addr_out) ? "X":"O") << "-> MINER: " << strAddr << std::endl;

					if(strAddr.compare(str_addr_out) == 0)
					{
						++miningContinuity;
						continue;
					}
					else
					{
						break;
					}
				}
			}
		}

	}

	return miningContinuity;
}

std::string GetCoinbaseAddress(CPubKey pkey)
{
	std::string strAddr="";

	strAddr = CBitcoinAddress(pkey.GetID()).ToString();

	return strAddr;
}

std::string GetCoinbaseAddress(const CBlock& block)
{
	std::string strAddr="";
	if(block.vtx.size()>0 && block.vtx[0].IsCoinBase())
	{
		const CTxOut& txout = block.vtx[0].vout[0];

		txnouttype whichType;
		const CScript& prevScript = txout.scriptPubKey;
		vector<CTxDestination> addressRets;
		int nRequiredRet;

		if(!ExtractDestinations(prevScript,whichType,addressRets,nRequiredRet))
		{
			std::string msg = "ExtractDestination() fail.";
			LogPrintf("hdac: %s\n", msg);
			//if(fDebug)std::cout << msg << std::endl;
		}
		else
		{
			if(addressRets.size()>0)
			{
				const CTxDestination &addr = addressRets[0];
				strAddr = CBitcoinAddress(addr).ToString();
			}
		}
	}

	return strAddr;
}

void PrintRecentBlockInfo(int depth)
{
	// CIH
	//CBlockIndex* pindexPrev = chainActive.Tip();
	int i=0;
	int mx_idx=3;

	std::cout << std::endl << std::endl << std::endl;

	for (CBlockIndex* pindex = chainActive.Tip();
		   i<mx_idx && pindex && pindex->pprev;
		   pindex = pindex->pprev, i++)
	{
		CPubKey minerPubkey = pindex->kMiner;
		CBitcoinAddress addr=CBitcoinAddress(minerPubkey.GetID());

		//CBlockHeader blockHeader = pindex->GetBlockHeader();
		std::string strBlockHash = pindex->GetBlockHash().GetHex();
		uint256 hash(strBlockHash);
		int nTx = pindex->nTx;

		CBlock block;
		if(!ReadBlockFromDisk(block, pindex))
		{
			  std::cout << "Can't read block from disk" << std::endl;

			  return ;
		}


		string str_addr_in;
		string str_addr_out;

		if(block.vtx.size()>0 && block.vtx[0].IsCoinBase())
		{
			//CTransaction vtx;
			const CTxOut& txout = block.vtx[0].vout[0];
			//CTxIn txin;

			txnouttype whichType;
			const CScript& prevScript = txout.scriptPubKey;
			vector<CTxDestination> addressRets;
			int nRequiredRet;

			if(!ExtractDestinations(prevScript,whichType,addressRets,nRequiredRet))
			{
				string msg = "\tExtractDestination() fail.";
				std::cout << msg << endl;
			}
			else
			{
				if(addressRets.size()>0)
				{
					const CTxDestination &addr = addressRets[0];
					str_addr_out = CBitcoinAddress(addr).ToString();
				}
			}
		}

		std::cout << "Height      : " << pindex->nHeight << " nTx:"<<nTx << std::endl;
		std::cout << "PUB KEY     : "<< minerPubkey.GetID().ToString() << endl;
		std::cout << "Miner       : " << addr.ToString() << std::endl;
		std::cout << "address_out : " << str_addr_out.c_str() << std::endl;
		std::cout << "address_in  : " << str_addr_in.c_str() << std::endl;
		//std::cout << "Block : " << block.ToString() << std::endl;
		//std::cout << "TX : " << vtx.ToString() << std::endl;
		//std::cout << "txout[0] : " << txout.ToString() << std::endl;

		std::cout << std::endl;

	}
}

void PrintBlockInfo(CNode* pfrom, CBlock* pblock)	// CIH
{
	string strFrom = "";
	if(pfrom==NULL)
	{
		strFrom = "<<\t";
	}

	std::cout << std::endl << std::endl << std::endl;
	PrintRecentBlockInfo(3);

	int nf= GetNodeFactor(NODEFACTOR_DEFAULT_DEPTH);
	int wz = GetBlockWindowSize();

	 // CIH
	std::cout << std::endl << std::endl << std::endl;
	const CBlock& block = *pblock;

	std::cout << strFrom << "Wz: " << wz << " NF:"<< nf << " H: " << chainActive.Tip()->nHeight << " tm:" << BLOCK_NUM_OF_MAX_BLOCKWINDOW << std::endl;
	std::cout << strFrom << "Block : " << block.ToString() << std::endl;
	const CBlockHeader & blockHeader = block.GetBlockHeader();

	if(block.vtx.size()>0)
	{
		const CTransaction &vtx = block.vtx[0];
		if(vtx.IsCoinBase()) // conibase
		{
			if(vtx.vin.size()>0)
			{
				BOOST_FOREACH (const CTxIn &in, vtx.vin)
				{
					const CScript& script = in.scriptSig;
				}

			}

			if(vtx.vout.size()>0)
			{
				const CTxOut & txout = vtx.vout[0];

				txnouttype whichType;
				const CScript& prevScript = txout.scriptPubKey;
				vector<CTxDestination> addressRets;
				int nRequiredRet;

				if(!ExtractDestinations(prevScript,whichType,addressRets,nRequiredRet))
				{
					string msg = "ExtractDestination() fail.";
					std::cout << msg << endl;
				}
				else
				{
					int i=0;
					BOOST_FOREACH (const CTxDestination &addr, addressRets)
					{
						string str = CBitcoinAddress(addr).ToString();
						//typedef boost::variant<CNoDestination, CKeyID, CScriptID> CTxDestination;
						const CKeyID *lpKeyID=boost::get<CKeyID> (&addr);
						const CScriptID *lpScriptID=boost::get<CScriptID> (&addr);
						if(lpKeyID!=NULL)
							std::cout << strFrom  << " " << "  PUB KEY     : " << lpKeyID->ToString() << std::endl;
						else
							std::cout << strFrom  << " " << "  PUB KEY     : " << "NULL" << std::endl;

						std::cout << strFrom  << " " << i++ << " address_out : " << str << std::endl;

					}
				}
			}

		}

		std::cout << std::endl;

   }
}

//bool IsAgainstEPow(CNode* pfrom, CBlock* pblock)
//{
//	bool fcheckBlockWz = VerifyBlockWindow(*pblock, pfrom);
//
////	int wz=0, nf=0, bh=0;
////	GetCurrentBlockWindowInfo(wz, nf, bh);
//	std::string Miner = GetCoinbaseAddress(*pblock);
//	int nContinuity = GetMiningContinuity(Miner);
//
//	if ( !fcheckBlockWz && !IsMiningBlackout() && (nContinuity > 1) && !IsInitialBlockDownload())
//	{
//		setBlacklistBlocks.insert(Miner);
//		WriteBlacklistMinerToDisk(Miner);
//		return true;
//	}
//
//	return false;
//}

 bool CheckePoWRule(std::string strMinerAddress, int blockHeight)
 {
 	if(IsInitialBlockDownload())
 		return true;

 	int miningContinuity = GetMiningContinuity(strMinerAddress);
 	int wz=0, nf=0, bh=0;
 	GetCurrentBlockWindowInfo(wz, nf, bh);
// 	int nf = GetNodeFactor(NODEFACTOR_DEFAULT_DEPTH, blockHeight);
//  if(nf == -1)
// 		return true;
// 	int wz = GetBlockWindowSize(blockHeight);
//  if(wz == -1)
// 		return true;

 	if(miningContinuity > wz)
 	{
 		if(IsMiningBlackout())
 		{
 			if(fDebug)LogPrint("%s : check_ePoWRule(%s) FAIL. WZ: %d NF: %d ", __func__, strMinerAddress, wz, nf);
 			return false;
 		}
 		return true;
 	}
 	return true;
 }
