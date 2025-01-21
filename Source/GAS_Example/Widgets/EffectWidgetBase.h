// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "CommonLazyImage.h"
#include "CommonNumericTextBlock.h"
#include "CommonTextBlock.h"
#include "CommonUserWidget.h"
#include "Components/RetainerBox.h"

#include "EffectWidgetBase.generated.h"

class UCommonLazyImage;
class UCommonTextBlock;
class UEffectWidgetControllerBase;
/**
 * 
 */
UCLASS()
class GAS_EXAMPLE_API UEffectWidgetBase : public UCommonUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UEffectWidgetControllerBase> EffectController;
	
	FTimerHandle DurationUpdateTimer;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidgetOptional))
	TObjectPtr<URetainerBox> R_Retainer;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> T_Title;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> T_Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	TObjectPtr<UCommonLazyImage> I_Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidgetOptional))
	TObjectPtr<UCommonNumericTextBlock> T_StackCount;

	float RemainingDuration = 0.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidgetOptional))
	TObjectPtr<UCommonNumericTextBlock> T_Duration;

public:
	UFUNCTION(BlueprintCallable)
	void SetController(UEffectWidgetControllerBase* NewController);
	
	UFUNCTION(BlueprintCallable)
	void UpdateTitle(const FText NewTitle);

	UFUNCTION(BlueprintCallable)
	void UpdateDescription(const FText NewDescription);

	UFUNCTION(BlueprintCallable)
	void UpdateIcon(TSoftObjectPtr<UTexture2D> NewSoftTexture, TSoftObjectPtr<UMaterialInterface> SoftMaterial);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateStackCount(const int32 NewStackCount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateDuration(const float NewDuration);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateInhibition(const bool bIsInhibited);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnSourceEffectHandleChanged();
};
