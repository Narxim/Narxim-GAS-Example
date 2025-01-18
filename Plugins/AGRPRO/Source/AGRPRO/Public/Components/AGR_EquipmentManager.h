// Copyright Adam Grodzki All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Data/AGRTypes.h"
#include "Components/ActorComponent.h"

#include "AGR_EquipmentManager.generated.h"

struct FGameplayTag;

UCLASS(BlueprintType, Blueprintable, ClassGroup=("AGR"), meta=(BlueprintSpawnableComponent))
class AGRPRO_API UAGR_EquipmentManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, SaveGame, Category="AGR|Game Play")
    TArray<FEquipment> EquipmentList;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="AGR|Game Play")
    TMap<FName, AActor*> References;

public:
    UAGR_EquipmentManager();

    virtual void TickComponent(
        const float DeltaTime,
        const ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="AGR")
    UPARAM(DisplayName = "Has Items") bool GetAllItems(UPARAM(DisplayName = "Items") TArray<AActor*>& OutItems);

    /**
     * Equips an item in the specific slot.
     *
     * Before executing equip action, the following conditions will be checked:
     * 1) Slot exists
     * 2) New item is an item
     * 3) Tag is acceptable in this slot
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AGR")
    UPARAM(DisplayName = "Success") bool EquipItemInSlot(
        const FName Slot,
        AActor* ItemActor,
        UPARAM(DisplayName = "PreviousItem") AActor*& OutPreviousItem,
        UPARAM(DisplayName = "NewItem") AActor*& OutNewItem);

    /* Names need to be unique */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AGR")
    void SetupDefineSlots(UPARAM(DisplayName = "Equipment") const TArray<FEquipment> InEquipmentList);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AGR")
    UPARAM(DisplayName = "Success") bool UnEquipItemFromSlot(
        const FName Slot,
        UPARAM(DisplayName = "ItemUnequipped") AActor*& OutItemUnequipped);

    //~ TODO "FText& OutNote" should be an enum to signal the actual outcome
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AGR")
    UPARAM(DisplayName = "Success") bool UnEquipByReference(
        AActor* ItemActor,
        UPARAM(DisplayName = "Note") FText& OutNote);

    UFUNCTION(BlueprintCallable, Category="AGR")
    UPARAM(DisplayName = "Success") bool GetItemInSlot(const FName Slot, UPARAM(DisplayName = "Item") AActor*& OutItem);

    UFUNCTION(BlueprintCallable, Category="AGR")
    void SaveShortcutReference(const FName Key, AActor* Item);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="AGR")
    UPARAM(DisplayName = "Found") bool GetShortcutReference(
        const FName Key,
        UPARAM(DisplayName = "Actor") AActor*& OutActor);

protected:
    virtual void BeginPlay() override;
};