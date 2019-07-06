// Free to use / modify 2019 Narxim


#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "AttributeSetBase.generated.h"


UCLASS()
class GASTUTORIAL_API UAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:	
	UAttributeSetBase();

	// Our basic Health attribute.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_Health, Category = "Attributes | Health") FGameplayAttributeData Health;
	UFUNCTION() void OnRep_Health() { GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, Health); }
	static FGameplayAttribute AttributeHealth();

	// Our basic MaxHealth attribute.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes | MaxHealth") FGameplayAttributeData MaxHealth;
	UFUNCTION() void OnRep_MaxHealth() { GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, MaxHealth); }
	static FGameplayAttribute AttributeMaxHealth();

	// Our basic Stamina attribute.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_Stamina, Category = "Attributes | Stamina") FGameplayAttributeData Stamina;
	UFUNCTION() void OnRep_Stamina() { GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, Stamina); }
	static FGameplayAttribute AttributeStamina();

	// Our basic MaxStamina attribute.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_MaxStamina, Category = "Attributes | MaxStamina") FGameplayAttributeData MaxStamina;
	UFUNCTION() void OnRep_MaxStamina() { GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, MaxStamina); }
	static FGameplayAttribute AttributeMaxStamina();


	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData &Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;
};
