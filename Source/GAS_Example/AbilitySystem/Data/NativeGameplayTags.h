// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/GameplayTags/Public/NativeGameplayTags.h"

namespace NativeGameplayTags
{
	namespace CharacterTags
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Character_Type_PC);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Character_Type_NPC);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Character_State_BlockHealthRegen);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Effect_NoLevel);
	}
}
