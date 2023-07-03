// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <hash.h>
#include <primitives/block.h>
#include <sync.h>
#include <uint256.h>
#include <util.h>
#include <utilstrencodings.h>

uint256 CBlockHeader::GetHash() const
{
	//return SerializeHash(*this);
    if (nTime < 1651444217)
        return HashX16R(BEGIN(nVersion), END(nNonce), hashPrevBlock);
    else
        return KAWPOWHash_OnlyMix(*this);
}

uint256 CBlockHeader::GetHashFull(uint256& mix_hash) const
{
    if (nTime < 1651444217) {
        return HashX16R(BEGIN(nVersion), END(nNonce), hashPrevBlock);
    } else {
        return KAWPOWHash(*this, mix_hash);
    }
}

uint256 CBlockHeader::GetX16RHash() const
{
    return HashX16R(BEGIN(nVersion), END(nNonce), hashPrevBlock);
}

/**
 * @brief This takes a block header, removes the nNonce64 and the mixHash. Then performs a serialized hash of it SHA256D.
 * This will be used as the input to the KAAAWWWPOW hashing function
 * @note Only to be called and used on KAAAWWWPOW block headers
 */
uint256 CBlockHeader::GetKAWPOWHeaderHash() const
{
    CKAWPOWInput input{*this};

    return SerializeHash(input);
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}
