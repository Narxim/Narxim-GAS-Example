// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectUIData.h"
#include "CustomGameplayEffectUIData.generated.h"

class UEffectWidgetControllerBase;
struct FActiveGameplayEffectHandle;

// https://unrealcommunity.wiki/bitmask-bitflag-enums-kdq2na6w
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ECustomGEDisplayParameter:uint8
{
	NONE			   = 0 UMETA(Hidden),
	HIDE_COOLDOWN	   = 1 << 0
};
ENUM_CLASS_FLAGS(ECustomGEDisplayParameter);

/**
 * UI data showing both name, description and an icon.
 * Can add more information to handle stacking behavior etc if required ...
 */
UCLASS(DisplayName="Custom UI Data", Config = Game)
class GAS_EXAMPLE_API UCustomGameplayEffectUIData : public UGameplayEffectUIData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (MultiLine = "true"))
	FText Title;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (MultiLine = "true"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data)
	TSoftObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data)
	TSoftObjectPtr<UMaterialInterface> IconMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data)
	TSubclassOf<UEffectWidgetControllerBase> EffectWidgetController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Parameters", meta = (Bitmask, BitmaskEnum = ECustomGEDisplayParameter))
	int32 UIParametersBitMask = 0;
	
	UFUNCTION(BlueprintPure, Category = "Custom Ability System")
	static const UCustomGameplayEffectUIData* GetCustomGameplayEffectUIData(FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(BlueprintPure, Category = "Custom Ability System")
	static const UCustomGameplayEffectUIData* GetCustomGameplayEffectUIDataFromActiveEffect(const FActiveGameplayEffect& ActiveEffect);

	UFUNCTION(BlueprintPure, Category = "UI Parameters")
	bool ShouldHideCooldown() const
	{
		return UIParametersBitMask & static_cast<uint8>(ECustomGEDisplayParameter::HIDE_COOLDOWN);
	}

	UFUNCTION(BlueprintCallable)
	bool MatchingParameters(UPARAM(meta = (Bitmask, BitmaskEnum = ECustomGEDisplayParameter)) int32 Bitmask)
	{
		return (UIParametersBitMask & Bitmask) == Bitmask;
	}
};
