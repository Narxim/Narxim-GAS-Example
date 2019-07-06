// Free to use / modify 2019 Narxim


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AttributeSetBase.h"
#include "PlayerStateBase.generated.h"


UCLASS()
class GASTUTORIAL_API APlayerStateBase : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	APlayerStateBase();

	// Our ability system component.
	UPROPERTY(BlueprintReadOnly, Category = "Player State Base | Ability System Component")
		class UAbilitySystemComponent* AbilitySystemComponent;

	// Our attribute set.
	UPROPERTY(BlueprintReadOnly, Category = "Player State Base | Attribute Set")
		class UAttributeSetBase* AttributeSet;

	// The ability in our "Ability 1" slot.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability System | Ability 2")
		class TSubclassOf<UGameplayAbility> Ability1;

	// The ability in our "Ability 2" slot.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability System | Ability 1")
		class TSubclassOf<UGameplayAbility> Ability2;

	// Helper function to get our ability system component (Used by the IAbilitySystemInterface).
	UFUNCTION(BlueprintPure, Category = "Player State Base | Ability System Component")
		UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Helper function that returns our attribute set.
	UFUNCTION(BlueprintPure, Category = "Player State Base | Attribute Set")
		UAttributeSetBase* GetAttributeSet() const;

	// Gives out ability system component the designated abilities in slots 1 and 2.
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State Base | Ability System Component")
		void GiveAbilities();

	// Gives us the functionality to add abilities via BP. (These will need to be activated via "Try activate Ability by Class" in BP)
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State Base | Ability System Component")
		void GiveGameplayAbility(TSubclassOf<UGameplayAbility> AbilityToGive);

private:
	// Called when the game starts or when spawned.
	void BeginPlay() override;
	
};
