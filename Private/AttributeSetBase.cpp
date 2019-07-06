// Free to use / modify 2019 Narxim


#include "AttributeSetBase.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "UnrealNetwork.h"

// Sets our default values for attributes.
UAttributeSetBase::UAttributeSetBase()
{
	Health = FGameplayAttributeData(100.f);
	MaxHealth = FGameplayAttributeData(100.f);
	Stamina = FGameplayAttributeData(100.f);
	MaxStamina = FGameplayAttributeData(100.f);
}

// Helper function for attribute comparison. (EX: Can be used to check against this attribute during Pre / Post Gameplay Effect Execute)
FGameplayAttribute UAttributeSetBase::AttributeHealth()
{
	static UProperty* Property = FindFieldChecked<UProperty>(UAttributeSetBase::StaticClass(), GET_MEMBER_NAME_CHECKED(UAttributeSetBase, Health));
	return FGameplayAttribute(Property);
}

// Helper function for attribute comparison. (EX: Can be used to check against this attribute during Pre / Post Gameplay Effect Execute)
FGameplayAttribute UAttributeSetBase::AttributeMaxHealth()
{
	static UProperty* Property = FindFieldChecked<UProperty>(UAttributeSetBase::StaticClass(), GET_MEMBER_NAME_CHECKED(UAttributeSetBase, MaxHealth));
	return FGameplayAttribute(Property);
}

// Helper function for attribute comparison. (EX: Can be used to check against this attribute during Pre / Post Gameplay Effect Execute)
FGameplayAttribute UAttributeSetBase::AttributeStamina()
{
	static UProperty* Property = FindFieldChecked<UProperty>(UAttributeSetBase::StaticClass(), GET_MEMBER_NAME_CHECKED(UAttributeSetBase, Stamina));
	return FGameplayAttribute(Property);
}

// Helper function for attribute comparison. (EX: Can be used to check against this attribute during Pre / Post Gameplay Effect Execute)
FGameplayAttribute UAttributeSetBase::AttributeMaxStamina()
{
	static UProperty* Property = FindFieldChecked<UProperty>(UAttributeSetBase::StaticClass(), GET_MEMBER_NAME_CHECKED(UAttributeSetBase, MaxStamina));
	return FGameplayAttribute(Property);
}


bool UAttributeSetBase::PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data)
{
	return true;
}

// This is called before an attribute is changed. (Useful to clamp attributes to min / max values.)
void UAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{

}

// This is called after a GameplayEffect has ben executed. (Useful to check for things such as Health <=0 to handle death etc.)
void UAttributeSetBase::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data)
{
	Super::PostGameplayEffectExecute(Data);
}

// Set up our attributes for replication.
void UAttributeSetBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, MaxStamina, COND_None, REPNOTIFY_Always);
}
