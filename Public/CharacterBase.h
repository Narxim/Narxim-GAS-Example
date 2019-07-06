
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AttributeSetBase.h"
#include "CharacterBase.generated.h"

UENUM(BlueprintType) 
enum class AbilityInput : uint8
{
	Ability1	UMETA(DisplayName = "Ability1"),	// This will be bound via key bindings in engine as "Ability1"
	Ability2	UMETA(DisplayName = "Ability2"),	// This will be bound via key bindings in engine as "Ability2"
};

UCLASS()
class GASTUTORIAL_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	// Sets default values for this character's properties.
	ACharacterBase();

	// Our ability system component.
	UPROPERTY(BlueprintReadOnly, Category = "Character Base | Ability System Component")
		class UAbilitySystemComponent* AbilitySystemComponent;

	// Helper function to get our ability system component (Used by the IAbilitySystemInterface).
	UFUNCTION(BlueprintPure, Category = "Character Base | Ability System Component")
		UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Our attribute set (From player state).
	UPROPERTY(BlueprintReadOnly, Category = "Character Base | Attribute Set")
		class UAttributeSetBase* AttributeSet;

protected:
	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	// Called when our controller changes.
	void PossessedBy(AController* NewController) override;

	// Called when our PlayerState is assigned.
	void OnRep_PlayerState() override;

	// Sets up the ability system component.
	void SetAbilitySystemComponent();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
