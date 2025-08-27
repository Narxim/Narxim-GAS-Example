// Copyright 2021 Joseph "Narxim" Thigpen.
// Copyright 2025 #bdc_patrick From Unreal Source (Discord) for the source.
// Copyright 2025 #JeanChene Modified version of BDC Patrick. Thanks to them !

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayEffectTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BPGameplayEffectExecutionCalculation.generated.h"

/**
 * @struct FBP_ExecutionParams
 * @brief A Blueprint-accessible version of FGameplayEffectCustomExecutionParameters.
 * Contains all relevant data for the calculation of a GameplayEffect.
 */
USTRUCT(BlueprintType)
struct FBP_ExecutionParams
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Execution Params")
	UAbilitySystemComponent* SourceAbilitySystemComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Execution Params")
	UAbilitySystemComponent* TargetAbilitySystemComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Execution Params")
	UObject* SourceObject = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Execution Params")
	AActor* SourceAvatar = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Execution Params")
	AActor* TargetAvatar = nullptr;

	FBP_ExecutionParams()
	{
	}
};


/**
 * @struct FBP_ModifierInfo
 * @brief A Blueprint-accessible version of FGameplayModifierInfo.
 * Used to define an attribute modification returned from a Blueprint calculation.
 */
USTRUCT(BlueprintType)
struct FBP_ModifierInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Modifier Info")
	TEnumAsByte<EGameplayModOp::Type> ModifierOp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Modifier Info")
	float Magnitude;

	FBP_ModifierInfo()
		: ModifierOp(EGameplayModOp::Additive),
		  Magnitude(0.f)
	{
	}
};

/**
 * @struct FBP_CapturedAttribute
 * @brief Contains the broken-down values of a captured attribute.
 */
USTRUCT(BlueprintType)
struct FBP_CapturedAttribute
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Captured Attribute")
	FGameplayAttribute Attribute;

	UPROPERTY(BlueprintReadOnly, Category = "Captured Attribute")
	float AttributeValue;

	FBP_CapturedAttribute()
		: AttributeValue(0.f)
	{
	}
};

/**
 * @class UBPGameplayEffectExecutionCalculation
 * @brief A GameplayEffectExecutionCalculation that allows implementing the core logic in Blueprints.
 */
UCLASS()
class GAS_EXAMPLE_API UBPGameplayEffectExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UBPGameplayEffectExecutionCalculation();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput)
	const override;

	/**
	* @brief Converts the C++ FGameplayEffectCustomExecutionParameters into the Blueprint-friendly FBP_ExecutionParams.
	*/
	UFUNCTION(BlueprintPure, Category = "ExecutionCalculation|Utilities", meta=(DisplayName = "Convert Execution Params to BP"))
	static void ConvertExecutionParams(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		UPARAM(ref)	FBP_ExecutionParams& OutParams);

protected:
	// Updated by GetCapturedAttributeValues to avoid sending a copy of the map.
	mutable TMap<FGameplayAttribute,FBP_CapturedAttribute> CapturedAttributeValues{};
	
public:
	/**
	 * @brief Extracts the values of the captured attributes and returns them in a Blueprint-friendly format.
	 * @param Exec Self Execution
	 * @param ExecutionParams The native C++ execution parameters.
	 * @param AttributesToCapture The array of attribute definitions to capture using default tags
	 * @return A map of Attribute and Value containing the captured attributes and their values.
	 */
	UFUNCTION(BlueprintPure, Category = "ExecutionCalculation|Utilities", meta=(DisplayName = "Break Relevant Attributes To Capture", DefaultToSelf="Exec"))
	static const TMap<FGameplayAttribute, FBP_CapturedAttribute>& BreakRelevantAttributesToCapture(
		const UBPGameplayEffectExecutionCalculation* Exec,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		const TArray<FGameplayEffectAttributeCaptureDefinition>& AttributesToCapture);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ExecutionCalculation")
	void BPExecute(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		const FBP_ExecutionParams& BPExecutionParams,
		const FGameplayEffectSpec& Spec,
		const FGameplayEffectContextHandle& EffectContextHandle,
		const FGameplayTagContainer& SourceTags,
		const FGameplayTagContainer& TargetTags,
		UPARAM(ref) FGameplayEffectCustomExecutionOutput& OutExecutionOutput
		) const;
	
	UFUNCTION(BlueprintCallable, Category = "ExecutionCalculation")
	static void AddOutputModifier(
		UPARAM(ref) FGameplayEffectCustomExecutionOutput& ExecutionOutput,
		FGameplayAttribute Attribute,
		TEnumAsByte<EGameplayModOp::Type> ModifierOp,
		float Magnitude
		);
};