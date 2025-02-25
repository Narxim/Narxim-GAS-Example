// Copyright 2021 Joseph "Narxim" Thigpen.


#include "AbilitySystemData.h"


bool FCustomGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
    bool bCombinedSuccess = FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

    enum RepFlag
    {
        REP_CustomContextData,
        REP_MAX
    };

    uint16 RepBits = 0;
    if (Ar.IsSaving())
    {
        if (!CustomContextData.IsEmpty())
        {
            RepBits |= 1 << REP_CustomContextData;
        }
    }

    Ar.SerializeBits(&RepBits, REP_MAX);
    if (RepBits & (1 << REP_CustomContextData))
    {
        bCombinedSuccess &= SafeNetSerializeTArray_WithNetSerialize<31>(Ar, CustomContextData, Map);

    }

    return bCombinedSuccess;
}

