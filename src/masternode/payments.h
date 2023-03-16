// Copyright (c) 2014-2023 The Dash Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_MASTERNODE_PAYMENTS_H
#define BITCOIN_MASTERNODE_PAYMENTS_H

#include <amount.h>

#include <string>
#include <vector>

class CGovernanceManager;
class CMasternodePayments;
class CBlock;
class CTransaction;
struct CMutableTransaction;
class CSporkManager;
class CTxOut;

/// TODO: all 4 functions do not belong here really, they should be refactored/moved somewhere (main.cpp ?)
bool IsBlockValueValid(const CSporkManager& sporkManager, CGovernanceManager& governanceManager,
                       const CBlock& block, int nBlockHeight, CAmount blockReward, std::string& strErrorRet);
bool IsBlockPayeeValid(const CSporkManager& sporkManager, CGovernanceManager& governanceManager,
                       const CTransaction& txNew, int nBlockHeight, CAmount blockReward);
void FillBlockPayments(const CSporkManager& sporkManager, CGovernanceManager& governanceManager,
                       CMutableTransaction& txNew, int nBlockHeight, CAmount blockReward, std::vector<CTxOut>& voutMasternodePaymentsRet, std::vector<CTxOut>& voutSuperblockPaymentsRet);

extern CMasternodePayments mnpayments;

//
// Masternode Payments Class
// Keeps track of who should get paid for which blocks
//

class CMasternodePayments
{
public:
    static bool GetBlockTxOuts(int nBlockHeight, CAmount blockReward, std::vector<CTxOut>& voutMasternodePaymentsRet);
    static bool IsTransactionValid(const CTransaction& txNew, int nBlockHeight, CAmount blockReward);

    static bool GetMasternodeTxOuts(int nBlockHeight, CAmount blockReward, std::vector<CTxOut>& voutMasternodePaymentsRet);
};

#endif // BITCOIN_MASTERNODE_PAYMENTS_H
