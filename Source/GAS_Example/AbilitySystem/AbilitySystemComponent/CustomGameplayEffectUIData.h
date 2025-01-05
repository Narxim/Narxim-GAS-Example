// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectUIData.h"
#include "CustomGameplayEffectUIData.generated.h"

class UEffectWidgetControllerBase;
struct FActiveGameplayEffectHandle;
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

	UFUNCTION(BlueprintPure, Category = "Custom Ability System")
	static const UCustomGameplayEffectUIData* GetCustomGameplayEffectUIData(FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(BlueprintPure, Category = "Custom Ability System")
	static const UCustomGameplayEffectUIData* GetCustomGameplayEffectUIDataFromActiveEffect(const FActiveGameplayEffect& ActiveEffect);
};
