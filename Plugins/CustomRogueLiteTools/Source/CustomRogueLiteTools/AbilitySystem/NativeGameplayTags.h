﻿// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/GameplayTags/Public/NativeGameplayTags.h"

namespace CRLTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_CRL_Buff_Backstab);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_CRL_Modifier_Backstab);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_CRL_Immunity_Backstab);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_CRL_Identifier_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_CRL_Identifier_Resistance);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_CRL_Identifier_Critic);
}
