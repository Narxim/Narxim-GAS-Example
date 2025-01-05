// Copyright 2021 Joseph "Narxim" Thigpen.


#include "EffectWidgetBase.h"

void UEffectWidgetBase::UpdateDescription(const FText NewDescription)
{
	T_Description->SetText(NewDescription);
	if (R_Retainer)
	{
		R_Retainer->RequestRender();
	}
}

void UEffectWidgetBase::UpdateIcon(const TSoftObjectPtr<UTexture2D> NewSoftTexture, TSoftObjectPtr<UMaterialInterface> SoftMaterial)
{
	if (!SoftMaterial.IsNull())
	{
		I_Icon->SetBrushFromSoftMaterial(SoftMaterial);
	} else if (!NewSoftTexture.IsNull())
	{
		I_Icon->SetBrushFromSoftTexture(NewSoftTexture);
	}
	
	if (R_Retainer)
	{
		R_Retainer->RequestRender();
	}
}

void UEffectWidgetBase::UpdateStackCount(const int32 NewStackCount)
{
	T_StackCount->SetCurrentValue(NewStackCount);
	T_StackCount->SetVisibility(NewStackCount > 1 ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (R_Retainer)
	{
		R_Retainer->RequestRender();
	}
}

void UEffectWidgetBase::UpdateDuration(const float NewDuration)
{
	if (!GetWorld())
	{
		return;
	}
	
	T_Duration->SetCurrentValue(NewDuration);
	ESlateVisibility NewVisibility = ESlateVisibility::Collapsed;
	
	if (NewDuration > 0.f)
	{
		NewVisibility = ESlateVisibility::SelfHitTestInvisible;
	}
	
	T_Duration->SetVisibility(NewVisibility);
	if (R_Retainer)
	{
		R_Retainer->RequestRender();
	}
}

void UEffectWidgetBase::UpdateTitle(const FText NewTitle)
{
	T_Title->SetText(NewTitle);
	if (R_Retainer)
	{
		R_Retainer->RequestRender();
	}
}
