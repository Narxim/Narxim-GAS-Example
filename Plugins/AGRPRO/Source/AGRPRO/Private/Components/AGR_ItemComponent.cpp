// Copyright Adam Grodzki All Rights Reserved.

#include "Components/AGR_ItemComponent.h"

// =============================================================================
// These includes are needed to prevent plugin build failures.
#include "TimerManager.h"
// =============================================================================

#include "Components/AGR_EquipmentManager.h"
#include "Components/AGR_InventoryManager.h"
#include "Data/AGRLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetGuidLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

const FName UAGR_ItemComponent::TAG_ITEM = FName("Item");

UAGR_ItemComponent::UAGR_ItemComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;

    SetIsReplicatedByDefault(true);
    UAGR_ItemComponent::SetAutoActivate(true);
}

void UAGR_ItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, ItemId);
    DOREPLIFETIME(ThisClass, InventoryId);
    DOREPLIFETIME(ThisClass, OwnerId);
    DOREPLIFETIME(ThisClass, bStackable);
    DOREPLIFETIME(ThisClass, MaxStack);
    DOREPLIFETIME(ThisClass, CurrentStack);
    DOREPLIFETIME(ThisClass, Weight);
    DOREPLIFETIME(ThisClass, Volume);
    DOREPLIFETIME(ThisClass, SpaceSlots);
    DOREPLIFETIME(ThisClass, ItemName);
    DOREPLIFETIME(ThisClass, bSimulateWhenDropped);
    DOREPLIFETIME(ThisClass, ItemTagSlotType);
}

void UAGR_ItemComponent::BeginPlay()
{
    Super::BeginPlay();

    /* No id = new random id */
    if(UKismetSystemLibrary::IsServer(this))
    {
        if(!ItemId.IsValid())
        {
            ItemId = UKismetGuidLibrary::NewGuid();
        }
    }

    AActor* ItemComponentOwner = GetOwner();
    if(!ensure(IsValid(ItemComponentOwner)))
    {
        return;
    }

    /* We must use owner relevancy to get loaded into memory with the owner */
    ItemComponentOwner->bNetUseOwnerRelevancy = true;

    /* Tag item for easier queries - tags are not replicated so watch out what is "server=true" and what is just begin play */
    ItemComponentOwner->Tags.AddUnique(TAG_ITEM);
}

void UAGR_ItemComponent::HideShowItem(const bool bHide) const
{
    AActor* ItemComponentOwner = GetOwner();
    if(!IsValid(ItemComponentOwner) || !ItemComponentOwner->HasAuthority())
    {
        return;
    }

    if(bHide)
    {
        UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ItemComponentOwner->GetRootComponent());
        if(IsValid(PrimitiveComponent))
        {
            PrimitiveComponent->SetSimulatePhysics(false);
        }
    }
    else
    {
        const FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
        ItemComponentOwner->DetachFromActor(DetachmentRules);
    }

    ItemComponentOwner->SetActorHiddenInGame(bHide);
    ItemComponentOwner->SetActorEnableCollision(!bHide);

    OnHiddenShown.Broadcast(bHide);
}

void UAGR_ItemComponent::EquipInternal(const FName Slot) const
{
    AActor* ItemActor = GetOwner();
    if(!IsValid(ItemActor) || !ItemActor->HasAuthority())
    {
        return;
    }

    AActor* ParentItemActor = ItemActor->GetOwner();
    if(!IsValid(ParentItemActor))
    {
        return;
    }

    const FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
    ItemActor->DetachFromActor(DetachmentRules);

    HideShowItem(false);

    UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ItemActor->GetRootComponent());
    if(IsValid(PrimitiveComponent))
    {
        PrimitiveComponent->SetSimulatePhysics(false);
    }

    OnEquip.Broadcast(ParentItemActor, Slot);
}

void UAGR_ItemComponent::UnEquipInternal(const FName Slot) const
{
    AActor* ItemComponentOwner = GetOwner();
    if(!IsValid(ItemComponentOwner) || !ItemComponentOwner->HasAuthority())
    {
        return;
    }

    AActor* ItemOwner = ItemComponentOwner->GetOwner();
    if(!IsValid(ItemOwner))
    {
        return;
    }

    UAGR_InventoryManager* InventoryManager = UAGRLibrary::GetInventory(ItemOwner);
    if(!IsValid(InventoryManager))
    {
        return;
    }

    InventoryManager->SetupInventoryStorageReference();
    if(!IsValid(InventoryManager->InventoryStorage))
    {
        return;
    }

    const FAttachmentTransformRules AttachmentRules(
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::KeepWorld,
        false);
    ItemComponentOwner->AttachToActor(InventoryManager->InventoryStorage, AttachmentRules, NAME_None);
    HideShowItem(true);
    OnUnEquip.Broadcast(ItemOwner, Slot);
}

void UAGR_ItemComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAGR_ItemComponent::PickUpItem(UAGR_InventoryManager* InventoryPicking)
{
    AActor* ItemActor = GetOwner();
    if(!IsValid(ItemActor) || !ItemActor->HasAuthority())
    {
        return;
    }

    if(!IsValid(InventoryPicking))
    {
        return;
    }

    /* Only do for items not already in same inventory */
    if(InventoryId == InventoryPicking->InventoryId)
    {
        if(InventoryPicking->bDebug)
        {
            const FString Msg = "Item already in some inventory!";
            GEngine->AddOnScreenDebugMessage(
                -1,
                2.0f,
                FColor::FromHex("00A8FFFF"),
                Msg);
            UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
        }

        return;
    }

    // Check for valid inventory owner as actor, to set ownership of items
    AActor* InventoryOwner = InventoryPicking->GetOwner();
    if(!IsValid(InventoryOwner))
    {
        return;
    }

    /* Different logic for stacking */
    if(bStackable)
    {
        /* Stack items in inventory */

        FText Note;
        const bool bSuccess = InventoryPicking->AddItemsOfClass(ItemActor->GetClass(), CurrentStack, Note);

        if(InventoryPicking->bDebug)
        {
            const FString Msg = FString::Printf(
                TEXT("Pick up item %s -- %s"),
                bSuccess ? TEXT("True") : TEXT("False"),
                *Note.ToString());
            GEngine->AddOnScreenDebugMessage(
                -1,
                2.0f,
                FColor::FromHex("00A8FFFF"),
                Msg);
            UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
        }

        if(bSuccess)
        {
            // Set ownership and instigator for the item - so the dispatcher can notify the owner of the update.
            ItemActor->SetOwner(InventoryOwner);
            ItemActor->SetInstigator(InventoryOwner->GetInstigator());
            OnPickup.Broadcast(InventoryPicking);
            ItemActor->Destroy();
        }
    }
    else
    {
        /* Non-fungible non-stackable pickup */

        HideShowItem(true);

        const AActor* ItemActorOwner = ItemActor->GetOwner();
        if(IsValid(ItemActorOwner))
        {
            // UnEquip item in previous equipment manager. An example for this use-case would be where
            // one player "steals" an item another player has already equipped.
            UAGR_EquipmentManager* EquipmentManagerOfItemOwner = UAGRLibrary::GetEquipment(ItemActorOwner);
            if(IsValid(EquipmentManagerOfItemOwner))
            {
                FText Note;
                EquipmentManagerOfItemOwner->UnEquipByReference(ItemActor, Note);
            }
        }

        ItemActor->SetOwner(InventoryOwner);
        ItemActor->SetInstigator(InventoryOwner->GetInstigator());

        InventoryPicking->SetupInventoryStorageReference();
        if(!IsValid(InventoryPicking->InventoryStorage))
        {
            return;
        }

        const FAttachmentTransformRules AttachmentRules(
            EAttachmentRule::SnapToTarget,
            EAttachmentRule::SnapToTarget,
            EAttachmentRule::KeepWorld,
            false);
        ItemActor->AttachToActor(InventoryPicking->InventoryStorage, AttachmentRules, NAME_None);

        /* We set only inventory not the owner. Fungible items might be "Stolen" or something */

        InventoryId = InventoryPicking->InventoryId;

        if(!OwnerId.IsValid())
        {
            OwnerId = InventoryPicking->InventoryId;
        }

        if(InventoryPicking->bDebug)
        {
            const FString Msg = "Item picked up";
            GEngine->AddOnScreenDebugMessage(
                -1,
                2.0f,
                FColor::FromHex("00A8FFFF"),
                Msg);
            UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
        }

        // Informs Inventory that a new item was picked up.
        InventoryPicking->OnItemUpdated.Broadcast(ItemActor);
        /* Handle pickup action in item for custom logic. */
        OnPickup.Broadcast(InventoryPicking);
    }
}

void UAGR_ItemComponent::DropItem()
{
    /* Uses equipment */

    AActor* ItemActor = GetOwner();
    if(!IsValid(ItemActor) || !ItemActor->HasAuthority())
    {
        return;
    }

    const AActor* ItemActorOwner = ItemActor->GetOwner();
    if(!IsValid(ItemActorOwner))
    {
        return;
    }

    // Try to unEquip item if equipment manager can be found
    UAGR_EquipmentManager* EquipmentManager = UAGRLibrary::GetEquipment(ItemActorOwner);
    if(IsValid(EquipmentManager))
    {
        /* Do an equipment check */

        TArray<AActor*> FoundItems;
        if(EquipmentManager->GetAllItems(FoundItems))
        {
            if(FoundItems.Contains(ItemActor))
            {
                FText Note;
                const bool bSuccess = EquipmentManager->UnEquipByReference(ItemActor, Note);

                const UAGR_InventoryManager* InventoryManager = UAGRLibrary::GetInventory(ItemActorOwner);
                if(IsValid(InventoryManager) && InventoryManager->bDebug)
                {
                    const FString Msg = FString::Printf(
                        TEXT("Unequip item: %s -- %s"),
                        bSuccess ? TEXT("True") : TEXT("False"),
                        *Note.ToString());
                    GEngine->AddOnScreenDebugMessage(
                        -1,
                        2.0f,
                        FColor::FromHex("00A8FFFF"),
                        Msg);
                    UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
                }
            }
        }
    }

    HideShowItem(false);

    /* Position of drop need to be set by owner but init position in case he forget is the owner position. */

    FHitResult HitResult;
    ItemActor->SetActorLocationAndRotation(
        ItemActorOwner->GetActorLocation(),
        ItemActorOwner->GetActorRotation(),
        false,
        &HitResult,
        ETeleportType::TeleportPhysics);

    UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ItemActor->GetRootComponent());
    if(IsValid(PrimitiveComponent))
    {
        PrimitiveComponent->SetSimulatePhysics(bSimulateWhenDropped);
    }

    InventoryId.Invalidate();

    OnItemDropped.Broadcast();
}

void UAGR_ItemComponent::UseItem(AActor* User, FGameplayTag GameplayTag)
{
    const AActor* ItemComponentOwner = GetOwner();
    if(!IsValid(ItemComponentOwner) || !ItemComponentOwner->HasAuthority())
    {
        return;
    }

    OnItemUsed.Broadcast(User, GameplayTag);
}

bool UAGR_ItemComponent::SplitItems(const int32 SplitSize)
{
    if(!bStackable)
    {
        return false;
    }

    if(SplitSize >= CurrentStack)
    {
        return false;
    }

    UWorld* World = GetWorld();
    if(!ensure(IsValid(World)))
    {
        return false;
    }

    AActor* ItemActor = GetOwner();
    if(!IsValid(ItemActor) || !ItemActor->HasAuthority())
    {
        return false;
    }

    const AActor* ItemActorOwner = ItemActor->GetOwner();
    if(!IsValid(ItemActorOwner))
    {
        return false;
    }

    UAGR_InventoryManager* InventoryManager = UAGRLibrary::GetInventory(ItemActorOwner);
    if(!IsValid(InventoryManager))
    {
        return false;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    const AActor* ItemCDO = Cast<AActor>(ItemActor->StaticClass()->GetDefaultObject());
    FTransform NewTransform = ItemActor->GetActorTransform();
    NewTransform.SetScale3D(IsValid(ItemCDO) ? ItemCDO->GetActorScale() : FVector::OneVector);

    AActor* NewItemActor = World->SpawnActor(ItemActor->GetClass(), &NewTransform, SpawnParams);

    // Split stack among both item components
    CurrentStack -= SplitSize;
    UAGRLibrary::GetItemComponent(NewItemActor)->CurrentStack = SplitSize;

    InventoryManager->AddItemToInventoryDirectly(NewItemActor);
    InventoryManager->OnItemUpdated.Broadcast(ItemActor);

    FOnSplitStack.Broadcast(NewItemActor);

    return true;
}

void UAGR_ItemComponent::DestroyItem()
{
    AActor* ItemActor = GetOwner();
    if(!IsValid(ItemActor) || !ItemActor->HasAuthority())
    {
        return;
    }

    DropItem();
    OnDestroy.Broadcast();

    // Destroy actor after waiting for one frame
    FTimerHandle Handle;
    ItemActor->GetWorld()->GetTimerManager().SetTimer(
        Handle,
        [ItemActor]() { ItemActor->Destroy(); },
        0.001f,
        false);
}