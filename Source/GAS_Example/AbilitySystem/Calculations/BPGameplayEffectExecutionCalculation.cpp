// Copyright 2021 Joseph "Narxim" Thigpen.
// Copyright 2025 #bdc_patrick From Unreal Source (Discord) for the source.
// Copyright 2025 #JeanChene Modified version of BDC Patrick. Thanks to them !

#include "BPGameplayEffectExecutionCalculation.h"
#include "AbilitySystemComponent.h"

UBPGameplayEffectExecutionCalculation::UBPGameplayEffectExecutionCalculation()
{
}

void UBPGameplayEffectExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    FBP_ExecutionParams BlueprintParams{};
    ConvertExecutionParams(ExecutionParams, BlueprintParams);

    if (!BlueprintParams.SourceAbilitySystemComponent || !BlueprintParams.TargetAbilitySystemComponent)
    {
        return;
    }

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    TMap<FGameplayAttribute, FBP_ModifierInfo> BlueprintModifiers{};
    
    BPExecute(
        ExecutionParams,
        BlueprintParams,
        Spec,
        Spec.GetEffectContext(),
        *Spec.CapturedSourceTags.GetAggregatedTags(), // Always valid
        *Spec.CapturedSourceTags.GetAggregatedTags(), // Always valid
        OutExecutionOutput
        );
}

void UBPGameplayEffectExecutionCalculation::ConvertExecutionParams(const FGameplayEffectCustomExecutionParameters& ExecutionParams, UPARAM(ref) FBP_ExecutionParams& OutParams)
{
    OutParams.SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
    OutParams.TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();

    if (OutParams.SourceAbilitySystemComponent)
    {
        OutParams.SourceObject = OutParams.SourceAbilitySystemComponent->GetOwner();
        OutParams.SourceAvatar = OutParams.SourceAbilitySystemComponent->GetAvatarActor();
    }

    if (OutParams.TargetAbilitySystemComponent)
    {
        OutParams.TargetAvatar = OutParams.TargetAbilitySystemComponent->GetAvatarActor();
    }
}

const TMap<FGameplayAttribute, FBP_CapturedAttribute>& UBPGameplayEffectExecutionCalculation::BreakRelevantAttributesToCapture(
    const UBPGameplayEffectExecutionCalculation* Exec,
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    const TArray<FGameplayEffectAttributeCaptureDefinition>& AttributesToCapture
    )
{
    static TMap<FGameplayAttribute,FBP_CapturedAttribute> EmptyCapturedAttributes{};
    if (!Exec)
    {
        UE_LOG(LogAbilitySystemComponent, Warning, TEXT("UBPGameplayEffectExecutionCalculation::GetCapturedAttributeValues - Invalid Exec. Abort"));
        return EmptyCapturedAttributes;
    }
    
    FAggregatorEvaluateParameters EvaluationParameters{};
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    if (!EvaluationParameters.SourceTags || !EvaluationParameters.TargetTags)
    {
        UE_LOG(LogAbilitySystemComponent, Warning, TEXT("UBPGameplayEffectExecutionCalculation::GetCapturedAttributeValues - Invalid Tags. Abort."));
        return EmptyCapturedAttributes;
    }

    Exec->CapturedAttributeValues.Empty(AttributesToCapture.Num());
    // Could have tried to use ExecutionParams.GetOwningSpec().CapturedRelevantAttributes, but it's private ...
    
    // Loop through each attribute definition we want to capture.
    for (const FGameplayEffectAttributeCaptureDefinition& CaptureDef : AttributesToCapture)
    {
        FBP_CapturedAttribute& CapturedAttribute = Exec->CapturedAttributeValues.Add(CaptureDef.AttributeToCapture);
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, CapturedAttribute.AttributeValue);
    }

    return Exec->CapturedAttributeValues;
}

void UBPGameplayEffectExecutionCalculation::AddOutputModifier(UPARAM(ref) FGameplayEffectCustomExecutionOutput& ExecutionOutput, FGameplayAttribute Attribute, TEnumAsByte<EGameplayModOp::Type> ModifierOp, float Magnitude)
{
    ExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(Attribute, ModifierOp, Magnitude));
}

void UBPGameplayEffectExecutionCalculation::BPExecute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    const FBP_ExecutionParams& BPExecutionParams,
    const FGameplayEffectSpec& Spec,
    const FGameplayEffectContextHandle& EffectContextHandle,
    const FGameplayTagContainer& SourceTags,
    const FGameplayTagContainer& TargetTags,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
}
