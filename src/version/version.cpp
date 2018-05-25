// Copyright (c) 2014-2017 Coin Sciences Ltd
// MultiChain code distributed under the GPLv3 license, see COPYING file.
// Copyright (c) 2017 Hdac Technology AG
// Hdac code distributed under the GPLv3 license, see COPYING file.

#include "hdac/hdac.h"

#include "version/version.h"
#include "version/hdacversion.h"

const char* mc_State::GetVersion()
{
    return HDAC_BUILD_DESC;
}

const char* mc_State::GetFullVersion()
{
    return HDAC_FULL_VERSION;
}

int mc_State::GetNumericVersion()
{
	
    return HDAC_BUILD_DESC_NUMERIC;
}

int mc_State::GetWalletDBVersion()
{
    if(mc_gState->m_WalletMode & MC_WMD_ADDRESS_TXS)
    {
        if(mc_gState->m_WalletMode & MC_WMD_MAP_TXS)
        {
            return -1;                
        }
        else
        {
            return 2;                
        }
    }
    
    return 1;
}

int mc_State::GetProtocolVersion()
{
    return HDAC_PROTOCOL_VERSION;
}

const int mc_State::GetRevision()
{
  return HDAC_BUILD_REVISION;
}

const bool mc_State::PrevParamsHash(unsigned char* hash)
{  
  if(!strcmp((char*)PARAMS_HASH_0_56_0_TESTNET, (char*)hash) || 
     !strcmp((char*)PARAMS_HASH_0_56_0_MAINNET, (char*)hash))
  {
    return true;
  }

  return false;
}

