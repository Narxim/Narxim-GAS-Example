// Copyright 2021 Joseph "Narxim" Thigpen.


#include "NativeGameplayTags.h"


namespace NativeGameplayTags
{
	namespace CharacterTags
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Character_Type_PC, "Character.Type.PlayerCharacter", "A Gameplay Tag applied to Characters that are controlled by a local player.")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Character_Type_NPC, "Character.Type.NonPlayerCharacter", "A Gameplay Tag applied to Characters that are AI controlled.")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Character_State_BlockHealthRegen, "Character.State.BlockHealthRegen", "A Gameplay Tag applied to a Character that is used in Gameplay Effects to block health regeneration.")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Effect_NoLevel, "Effect.NoLevel", "This GE won't be leveled up or down. Base level will always be the same (upon granting it).")
	}
}