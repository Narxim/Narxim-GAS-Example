// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "AbilitySystemData.generated.h"


struct FCustomAttributeInitializer;
class UAttributeSet;
class UGameplayAbility;
class UGameplayEffect;

USTRUCT(Blueprintable)
struct FCustomAttributeMaxValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FScalableFloat MaxFloat{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute MaxAttribute{};
};

USTRUCT(Blueprintable)
struct FCustomAttributeInitializer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FScalableFloat Value;
};

// Contains data used to initialize an Ability System Component.
USTRUCT(BlueprintType)
struct FAbilitySystemInitializationData
{
	GENERATED_BODY()

	// An array of Attribute Sets to create.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSet<TSubclassOf<UAttributeSet>> AttributeSets;

	// A map of Attributes / float used to set base values.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FGameplayAttribute, FCustomAttributeInitializer> AttributeBaseValues;

	// An Array of Gameplay Abilities to give.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSet<TSubclassOf<UGameplayAbility>> GameplayAbilities;

	// An array of Gameplay Effects to apply.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSet<TSubclassOf<UGameplayEffect>> GameplayEffects;

	// A container of GameplayTags to apply.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer GameplayTags;
};

// Returns the value of an Attribute based on the search type.
UENUM(BlueprintType)
enum class EAttributeSearchType : uint8
{
	// Returns the final value of the Attribute including all stateful Gameplay Effect modifiers.
	FinalValue,

	// Returns the base value of the Attribute. (Excludes duration based Gameplay Effect modifiers)
	BaseValue,

	// Returns the Final Value minus the Base Value.
	BonusValue
};

USTRUCT(BlueprintType)
struct FCustomContextDataBase
{
	GENERATED_BODY()
	
	virtual ~FCustomContextDataBase() = default;
	
	/** Overridden to serialize new fields */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		return true;
	};
	
	FCustomContextDataBase() {}
};

template <>
struct TStructOpsTypeTraits<FCustomContextDataBase> : public TStructOpsTypeTraitsBase2<FCustomContextDataBase>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true // Necessary so that TSharedPtr<STRUCT> Data is copied around
	};
};


USTRUCT(BlueprintType)
struct FCustomGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
	const TArray<FInstancedStruct>& GetAllCustomContextData() const { return CustomContextData; }
	TArray<FInstancedStruct>& GetMutableAllCustomContextData() { return CustomContextData; }

protected:
	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TArray<FInstancedStruct> CustomContextData{};

	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TObjectPtr<const AActor> TargetActor;
 
public:

	void SetTargetActor(const AActor* InTargetActor)
	{
		TargetActor = InTargetActor;
	}

	const AActor* GetTargetActor() const
	{
		return TargetActor.Get();
	}

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	
	template <typename T>
		const T* FindCustomContextData() const
	{
		for (const auto& Fragment : CustomContextData)
		{
			if (const T* TypedFragment = Fragment.GetPtr<T>())
				return TypedFragment;
		}

		return nullptr;
	}

	/** Adds a Context Fragment to the ContextFragments array */
	template <typename T>
	void AddCustomContextData(const T& Fragment)
	{
		TInstancedStruct<FCustomContextDataBase> InstancedStruct;
		InstancedStruct.InitializeAs<T>();
		T& Mutable = InstancedStruct.GetMutable<T>();
		Mutable = Fragment;

		CustomContextData.Add(MoveTemp(InstancedStruct));
	}
	
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FCustomGameplayEffectContext::StaticStruct();
	}
 
	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FCustomGameplayEffectContext* Duplicate() const override
	{
		FCustomGameplayEffectContext* NewContext = new FCustomGameplayEffectContext();
		*NewContext = *this;
		NewContext->AddActors(Actors);
		
		NewContext->TargetActor = TargetActor;
		
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
};
 
template <>
struct TStructOpsTypeTraits<FCustomGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FCustomGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true // Necessary so that TSharedPtr<STRUCT> Data is copied around
	};
};


/** A "processed" version of GSGameplayEffectContainer that can be passed around and eventually applied */
USTRUCT(BlueprintType)
struct FGameplayEffectContainerSpec
{
	GENERATED_BODY()

public:
	FGameplayEffectContainerSpec() {}

	/** Computed target data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	FGameplayAbilityTargetDataHandle TargetData;

	/** List of gameplay effects to apply to the targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;

	/** Returns true if this has any valid effect specs */
	bool HasValidEffects() const;

	/** Returns true if this has any valid targets */
	bool HasValidTargets() const;

	/** Adds new targets to target data */
	void AddTargets(const TArray<FGameplayAbilityTargetDataHandle>& InTargetData, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	/** Clears target data */
	void ClearTargets();
};