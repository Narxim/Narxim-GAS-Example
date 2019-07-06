// Free to use / modify 2019 Narxim


#include "CharacterBase.h"
#include "PlayerStateBase.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Returns our ability system component.
UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Get the ability system component from the player state when assigned.
void ACharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	SetAbilitySystemComponent();
}

// Get the ability system component from the player controller when assigned.
void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	SetAbilitySystemComponent();
}

// Called to set up the character's ability system component.
void ACharacterBase::SetAbilitySystemComponent()
{
	if(IsValid(GetPlayerState()))
	{
		AbilitySystemComponent = Cast<APlayerStateBase>(GetPlayerState())->GetAbilitySystemComponent();
		AttributeSet = Cast<APlayerStateBase>(GetPlayerState())->GetAttributeSet();
		AbilitySystemComponent->InitAbilityActorInfo(GetPlayerState(), this);
	}
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds("ConfirmInput", "CancelInput", "AbilityInput"));
}