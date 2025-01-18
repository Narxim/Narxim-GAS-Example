// Copyright Adam Grodzki All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Components/AGRAnimMasterComponent.h"
#include "Components/AGR_InventoryManager.h"
#include "Components/AGR_ItemComponent.h"
#include "Components/AGR_SoundMaster.h"
#include "Components/AGR_CombatManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "AGRLibrary.generated.h"

UCLASS()
class AGRPRO_API UAGRLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    FORCEINLINE static UAGR_ItemComponent* GetItemComponent(const AActor* Actor)
    {
        return IsValid(Actor)
               ? Cast<UAGR_ItemComponent>(Actor->GetComponentByClass(UAGR_ItemComponent::StaticClass()))
               : nullptr;
    }

    FORCEINLINE static UAGR_InventoryManager* GetInventory(const AActor* Actor)
    {
        return IsValid(Actor)
               ? Cast<UAGR_InventoryManager>(Actor->GetComponentByClass(UAGR_InventoryManager::StaticClass()))
               : nullptr;
    }

    FORCEINLINE static UAGR_EquipmentManager* GetEquipment(const AActor* Actor)
    {
        return IsValid(Actor)
               ? Cast<UAGR_EquipmentManager>(Actor->GetComponentByClass(UAGR_EquipmentManager::StaticClass()))
               : nullptr;
    }

    FORCEINLINE static UAGRAnimMasterComponent* GetAnimationMaster(const AActor* Actor)
    {
        return IsValid(Actor)
               ? Cast<UAGRAnimMasterComponent>(Actor->GetComponentByClass(UAGRAnimMasterComponent::StaticClass()))
               : nullptr;
    }

    FORCEINLINE static UAGR_SoundMaster* GetSound(const AActor* Actor)
    {
        return IsValid(Actor)
               ? Cast<UAGR_SoundMaster>(Actor->GetComponentByClass(UAGR_SoundMaster::StaticClass()))
               : nullptr;
    }

    FORCEINLINE static UAGR_CombatManager* GetCombatManager(const AActor* Actor)
    {
        return IsValid(Actor)
               ? Cast<UAGR_CombatManager>(Actor->GetComponentByClass(UAGR_CombatManager::StaticClass()))
               : nullptr;
    }

private:
    UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Item Component", Category="AGR")
    static UAGR_ItemComponent* K2_GetItemComponent(AActor* Actor)
    {
        return GetItemComponent(Actor);
    };

    UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Inventory", Category="AGR")
    static UAGR_InventoryManager* K2_GetInventory(AActor* Actor)
    {
        return GetInventory(Actor);
    };

    UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Equipment Manager", Category="AGR")
    static UAGR_EquipmentManager* K2_GetEquipment(AActor* Actor)
    {
        return GetEquipment(Actor);
    };

    UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Animation Master", Category="AGR")
    static UAGRAnimMasterComponent* K2_GetAnimationMaster(AActor* Actor)
    {
        return GetAnimationMaster(Actor);
    };

    UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Sound Master", Category="AGR")
    static UAGR_SoundMaster* K2_GetSound(AActor* Actor)
    {
        return GetSound(Actor);
    };

    UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Combat Manager", Category="AGR")
    static UAGR_CombatManager* K2_GetCombatManager(AActor* Actor)
    {
        return GetCombatManager(Actor);
    };
};