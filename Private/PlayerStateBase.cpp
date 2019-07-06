
#include "PlayerStateBase.h"
#include "AttributeSetBase.h"

APlayerStateBase::APlayerStateBase()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
	AttributeSet = CreateDefaultSubobject<UAttributeSetBase>(TEXT("Attribute Set"));
}

void APlayerStateBase::BeginPlay()
{
	Super::BeginPlay();
	GiveDefaults();
}

// Returns our ability system component.
UAbilitySystemComponent* APlayerStateBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSetBase* APlayerStateBase::GetAttributeSet() const
{
	return AttributeSet;
}

// Checks to see if Ability1 and Ability 2 are valid and gives them to the ability system component.
void APlayerStateBase::GiveDefaults_Implementation()
{
	if(IsValid(AbilitySystemComponent))
	{
		if (IsValid(Ability1))
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability1.GetDefaultObject(), 1, 0));
		}
		if (IsValid(Ability2))
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability2.GetDefaultObject(), 1, 1));
		}
	}
}

bool APlayerStateBase::GiveDefaults_Validate()
{
	return true;
}
