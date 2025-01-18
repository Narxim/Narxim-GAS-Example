// Copyright Adam Grodzki All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"

#include "AGR_InventoryManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUpdated, AActor*, Item);

UCLASS(BlueprintType, Blueprintable, ClassGroup=("AGR"), meta=(BlueprintSpawnableComponent))
class AGRPRO_API UAGR_InventoryManager : public UActorComponent
{
    GENERATED_BODY()

    friend class UAGR_ItemComponent;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, SaveGame, Category="AGR")
    FGuid InventoryId;

    UPROPERTY(BlueprintReadWrite, Replicated, Category="AGR")
    AActor* InventoryStorage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AGR")
    bool bDebug;

    // Called whenever an item is updated inside the inventory.
    UPROPERTY(BlueprintAssignable, Category = "AGR|Events")
    FOnItemUpdated OnItemUpdated;

private:
    bool bHasPlayerState = false;

public:
    UAGR_InventoryManager();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void TickComponent(
        const float DeltaTime,
        const ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    /* If you need to set inventory id to player's id or something and force change of id. ID i "SaveGame" serialized. */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AGR")
    void OverwriteId(UPARAM(DisplayName = "InventoryId") const FGuid InInventoryId);

    //~ TODO "FText& OutNote" should be an enum to signal the actual outcome
    /* Only works for stackable items */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AGR")
    UPARAM(DisplayName = "Success") bool AddItemsOfClass(
        const TSubclassOf<AActor> Class,
        const int32 Quantity,
        FText& OutNote);

    //~ TODO "FText& OutNote" should be an enum to signal the actual outcome
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AGR")
    UPARAM(DisplayName = "Success") bool RemoveItemsOfClass(
        const TSubclassOf<AActor> Class,
        const int32 Quantity,
        FText& OutNote);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="AGR")
    UPARAM(DisplayName = "Items") TArray<AActor*> GetAllItems();

    UFUNCTION(BlueprintCallable, Category="AGR")
    UPARAM(DisplayName = "Found") bool GetAllItemsOfClass(
        const TSubclassOf<AActor> Class,
        UPARAM(DisplayName = "FilteredArray") TArray<AActor*>& OutFilteredArray);

    //~ TODO "FText& OutNote" should be an enum to signal the actual outcome
    UFUNCTION(BlueprintCallable, Category="AGR")
    UPARAM(DisplayName = "Success") bool HasEnoughItems(
        const TSubclassOf<AActor> Item,
        const int32 Quantity,
        FText& OutNote);

    UFUNCTION(BlueprintCallable, Category="AGR")
    UPARAM(DisplayName = "Success") bool GetAllItemsOfTagSlotType(
        const FGameplayTag SlotTypeFilter,
        TArray<AActor*>& OutItemsWithTag);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AGR")
    void AddItemToInventoryDirectly(AActor* Item);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="AGR")
    UPARAM(DisplayName = "Success") bool HasExactItem(AActor* Item);

protected:
    virtual void BeginPlay() override;

    /**
     * Basically, for pawns we store items on player state that replicates with the player itself.
     * if not, then it's most likely a container and we attach items to it.
     */
    void SetupInventoryStorageReference();
};