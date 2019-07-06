// Free to use / modify 2019 Narxim


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

	if(HasAuthority())
	{
		GiveAbilities();
	}
}

// Returns our ability system component.
UAbilitySystemComponent* APlayerStateBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Returns our attribute set.
UAttributeSetBase* APlayerStateBase::GetAttributeSet() const
{
	return AttributeSet;
}

// Checks to see if Ability1 and Ability 2 are valid and gives them to the ability system component.
void APlayerStateBase::GiveAbilities_Implementation()
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

bool APlayerStateBase::GiveAbilities_Validate()
{
	return true;
}

// Checks to make sure we have authority and an ability then gives the specified ability.
void APlayerStateBase::GiveGameplayAbility_Implementation(TSubclassOf<UGameplayAbility> AbilityToGive)
{
	if(HasAuthority() && AbilityToGive)
	{
		AbilitySystemComponent->GiveAbility(AbilityToGive);
	}
}

bool APlayerStateBase::GiveGameplayAbility_Validate(TSubclassOf<UGameplayAbility> AbilityToGive)
{
	return true;
}