// Copyright Adam Grodzki All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "AGR_EquipmentManager.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"

#include "AGR_ItemComponent.generated.h"

class UAGR_InventoryManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickup, UAGR_InventoryManager*, Inventory);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSplitStack, AActor*, NewItem);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHiddenShown, bool, bHidden);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemDropped);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUsed, AActor*, User, FGameplayTag, GameplayTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquip, AActor*, User, FName, SlotName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnEquip, AActor*, User, FName, SlotName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestroy);

UCLASS(BlueprintType, Blueprintable, ClassGroup=("AGR"), meta=(BlueprintSpawnableComponent))
class AGRPRO_API UAGR_ItemComponent : public UActorComponent
{
    GENERATED_BODY()

    friend UAGR_EquipmentManager;
    friend UAGR_InventoryManager;

public:
    static const FName TAG_ITEM;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, SaveGame, Category="AGR|Identification")
    FGuid ItemId;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, SaveGame, Category="AGR|Identification")
    FGuid InventoryId;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, SaveGame, Category="AGR|Identification")
    FGuid OwnerId;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="AGR|Quantity")
    bool bStackable = false;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="AGR|Quantity")
    int32 MaxStack = 1;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, SaveGame, Category="AGR|Quantity")
    int32 CurrentStack = 1;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="AGR|Inventory Space")
    float Weight = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="AGR|Inventory Space")
    float Volume = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="AGR|Inventory Space")
    int32 SpaceSlots = 1;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, SaveGame, Category="AGR|Base Info")
    FName ItemName = TAG_ITEM;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="AGR|Base Info")
    bool bSimulateWhenDropped = false;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="AGR|Base Info")
    FGameplayTag ItemTagSlotType;

    UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "AGR|Events")
    FOnPickup OnPickup;

    UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "AGR|Events")
    FOnSplitStack FOnSplitStack;

    UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "AGR|Events")
    FOnHiddenShown OnHiddenShown;

    UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "AGR|Events")
    FOnItemDropped OnItemDropped;

    UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "AGR|Events")
    FOnItemUsed OnItemUsed;

    UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "AGR|Events")
    FOnEquip OnEquip;

    UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "AGR|Events")
    FOnUnEquip OnUnEquip;

    UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "AGR|Events")
    FOnDestroy OnDestroy;

public:
    UAGR_ItemComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void TickComponent(
        const float DeltaTime,
        const ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AGR")
    void PickUpItem(UAGR_InventoryManager* InventoryPicking);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AGR")
    void DropItem();

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AGR")
    void UseItem(AActor* User, FGameplayTag GameplayTag);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AGR")
    bool SplitItems(const int32 SplitSize);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AGR")
    void DestroyItem();

protected:
    virtual void BeginPlay() override;

private:
    void HideShowItem(const bool bHide) const;
    void EquipInternal(const FName Slot) const;
    void UnEquipInternal(const FName Slot) const;
};