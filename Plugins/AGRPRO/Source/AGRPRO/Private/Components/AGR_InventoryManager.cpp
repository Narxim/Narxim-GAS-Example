// Copyright Adam Grodzki All Rights Reserved.

#include "Components/AGR_InventoryManager.h"

// =============================================================================
// These includes are needed to prevent plugin build failures.
#include "Engine/Engine.h"
// =============================================================================

#include "Components/AGR_ItemComponent.h"
#include "Data/AGRLibrary.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetGuidLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

UAGR_InventoryManager::UAGR_InventoryManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;

    SetIsReplicatedByDefault(true);
    UAGR_InventoryManager::SetAutoActivate(true);
}

void UAGR_InventoryManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, InventoryId);
    DOREPLIFETIME(ThisClass, InventoryStorage);
}

void UAGR_InventoryManager::BeginPlay()
{
    Super::BeginPlay();

    InventoryStorage = nullptr;

    /* No id = make new id */
    if(!InventoryId.IsValid())
    {
        InventoryId = UKismetGuidLibrary::NewGuid();
    }

    // AActor* XActor = GetOwner();
    // APawn* XPawn =  Cast<APawn>(GetOwner());

    bHasPlayerState = Cast<APawn>(GetOwner()) != nullptr;

    // UE_LOG(LogTemp, Warning, TEXT("Owner: %s"), *GetNameSafe(GetOwner()));
    UE_LOG(LogTemp, Warning, TEXT("bHasPlayerState: %s"), bHasPlayerState ? TEXT("true") : TEXT("false"));

    SetupInventoryStorageReference();
}

void UAGR_InventoryManager::SetupInventoryStorageReference()
{
    if(IsValid(InventoryStorage))
    {
        // Storage ref already set up
        return;
    }

    AActor* InventoryOwner = GetOwner();
    if(!IsValid(InventoryOwner))
    {
        return;
    }

    if(bHasPlayerState)
    {
        // Try to get a ref to the player state.
        const APawn* OwningPawn = Cast<APawn>(InventoryOwner);
        if(!IsValid(OwningPawn))
        {
            return;
        }

        /* We pick player state as best suited container as it doesn't move or rotate,
         * so doesn't net update with the pawn but is always relevant (still not the owner
         * of the attached actor!)
         */
        AActor* PlayerState = OwningPawn->GetPlayerState();
        if(IsValid(PlayerState))
        {
            InventoryStorage = PlayerState;
        }
    }
    else
    {
        InventoryStorage = InventoryOwner;
    }
}

void UAGR_InventoryManager::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAGR_InventoryManager::OverwriteId(const FGuid InInventoryId)
{
    if(UKismetSystemLibrary::IsServer(this))
    {
        InventoryId = InInventoryId;
    }
}

bool UAGR_InventoryManager::AddItemsOfClass(
    const TSubclassOf<AActor> Class,
    const int32 Quantity,
    UPARAM(DisplayName = "Note") FText& OutNote)
{
    /* This function allows actors of a different classes on interaction to add stacks of different actors.
     * For example, actor BP_TREE when punched can add 3 stacks of actor BP_WOOD for creating new items with a function.
     */
    AActor* InventoryManagerOwner = GetOwner();
    if(!IsValid(InventoryManagerOwner) || !InventoryManagerOwner->HasAuthority())
    {
        return false;
    }

    if(Quantity <= 0)
    {
        // Failed to add item to inventory
        OutNote = FText::FromString("Quantity must be greater than zero");
        return false;
    }

    int32 StacksToAdd = Quantity;

    /* Check if inventory already has this item */
    TArray<AActor*> FilteredArray;
    const bool bAlreadyHasItem = GetAllItemsOfClass(Class, FilteredArray);

    if(bAlreadyHasItem)
    {
        for(AActor* ItemActor : FilteredArray)
        {
            UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(ItemActor);
            if(!IsValid(ItemComponent))
            {
                continue;
            }

            // Is stacking items allowed?
            if(!ItemComponent->bStackable)
            {
                // Failed to add item to inventory
                OutNote = FText::FromString("Trying to stack non-fungible item!");
                return false;
            }

            // Free slots available?
            const int32 FreeSlotsAvailable = FMath::Max(0, ItemComponent->MaxStack - ItemComponent->CurrentStack);
            if(FreeSlotsAvailable <= 0)
            {
                continue;
            }

            if(FreeSlotsAvailable >= StacksToAdd)
            {
                /* This item will add all stacks */
                ItemComponent->CurrentStack = ItemComponent->CurrentStack + StacksToAdd;

                /* All stacks added */
                StacksToAdd = 0;
                OnItemUpdated.Broadcast(ItemActor);
                break;
            }

            /* Maxed out stack and continue ... */
            ItemComponent->CurrentStack = ItemComponent->MaxStack;

            /* Decrease remaining item count and continue loop */
            StacksToAdd -= FreeSlotsAvailable;
        }

        if(StacksToAdd <= 0)
        {
            // No more work to do.
            OutNote = FText::FromString("Stacks increased to current");

            // Successfully added item to inventory.
            return true;
        }
    }

    /* Loop */

    while(StacksToAdd > 0)
    {
        /* Notice inventory storage actor is not the owner.
         * OWNER of the inventory (preferably the pawn) is the owner and the instigator is the instigator of a new item.
         * Player state actor is always relevant, so if we inherit always relevant for all items it wil be a multiplayer cluster fuck.
         * Items are attached to an always relevant actor but gets net relevant with the pawn.
         */

        UWorld* World = GetWorld();
        if(!ensure(IsValid(World)))
        {
            return false;
        }

        SetupInventoryStorageReference();
        if(!IsValid(InventoryStorage))
        {
            return false;
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.Owner = InventoryManagerOwner;
        SpawnParams.Instigator = InventoryManagerOwner->GetInstigator();

        const AActor* ItemCDO = Cast<AActor>(Class->StaticClass()->GetDefaultObject());
        FTransform NewTransform = InventoryStorage->GetActorTransform();
        NewTransform.SetScale3D(IsValid(ItemCDO) ? ItemCDO->GetActorScale() : FVector::OneVector);
        AActor* NewItemActor = World->SpawnActor(Class, &NewTransform, SpawnParams);

        /* Do some stuff relevant to item */
        UAGR_ItemComponent* NewItemActorItemComponent = UAGRLibrary::GetItemComponent(NewItemActor);
        if(!IsValid(NewItemActorItemComponent))
        {
            return false;
        }

        NewItemActorItemComponent->InventoryId = InventoryId;

        /* Fungible stackable item's ownership is impossible to track if one is indistinguishable from another */
        NewItemActorItemComponent->OwnerId = InventoryId;

        NewItemActorItemComponent->HideShowItem(true);

        /* Attach to designated actor storage. In case of pawns, player state. (AI also has player state) */
        FAttachmentTransformRules AttachmentRules(
            EAttachmentRule::SnapToTarget,
            EAttachmentRule::SnapToTarget,
            EAttachmentRule::KeepWorld,
            false);
        NewItemActor->AttachToActor(InventoryStorage, AttachmentRules, NAME_None);

        if(NewItemActorItemComponent->MaxStack >= StacksToAdd)
        {
            /* Less than max? Add stacks... */
            NewItemActorItemComponent->CurrentStack = StacksToAdd;
            StacksToAdd = 0;
        }
        else
        {
            /* More items to spawn? Add max and reduce stacks */
            NewItemActorItemComponent->CurrentStack = NewItemActorItemComponent->MaxStack;
            StacksToAdd -= NewItemActorItemComponent->MaxStack;
        }
        OnItemUpdated.Broadcast(NewItemActor);
    }

    // Successfully added item to inventory
    OutNote = FText::FromString("New items spawned and registered to inventory");
    return true;
}

bool UAGR_InventoryManager::RemoveItemsOfClass(
    const TSubclassOf<AActor> Class,
    const int32 Quantity,
    UPARAM(DisplayName = "Note") FText& OutNote)
{
    /* Non-negative stacks */

    /* We don't trust users to do the check themselves before each removal */

    const AActor* InventoryManagerOwner = GetOwner();
    if(!IsValid(InventoryManagerOwner) || !InventoryManagerOwner->HasAuthority())
    {
        return false;
    }

    FText Note;
    const bool bHasEnoughItems = HasEnoughItems(
        Class,
        Quantity,
        Note);

    if(bDebug)
    {
        const FString Msg = FString::Printf(
            TEXT("Has enough items: %s"),
            bHasEnoughItems ? TEXT("True") : TEXT("False"));
        GEngine->AddOnScreenDebugMessage(
            -1,
            2.0f,
            FColor::FromHex("00A8FFFF"),
            Msg);
        UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
    }

    if(!bHasEnoughItems)
    {
        // Failed to remove items
        OutNote = FText::FromString("Stack to remove negative or 0");
        return false;
    }

    int32 StacksToRemove = Quantity;

    /* Has items of class in inventory. Now we just have to see if it's enough */

    TArray<AActor*> FilteredArray;
    if(!GetAllItemsOfClass(Class, FilteredArray))
    {
        // Failed to remove items
        OutNote = FText::FromString("No items of class");
        return false;
    }

    /* This item has more stacks than to remove ... */
    for(int32 i = FilteredArray.Num() - 1; i >= 0; --i)
    {
        AActor* ItemActor = FilteredArray[i];
        if(!IsValid(ItemActor))
        {
            continue;
        }

        UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(ItemActor);
        if(!IsValid(ItemComponent))
        {
            continue;
        }

        if(!ItemComponent->bStackable)
        {
            // Failed to remove items
            OutNote = FText::FromString("Not a fungible stackable item!");
            return false;
        }

        if(ItemComponent->CurrentStack > StacksToRemove)
        {
            /* Reduce stacks and break the loop */
            ItemComponent->CurrentStack -= StacksToRemove;
            StacksToRemove = 0;
        }
        else
        {
            /* Deplete all stacks, destroy item, and move along ... */
            StacksToRemove -= ItemComponent->CurrentStack;
            ItemActor->Destroy();
        }
    }

    // Successfully removed items
    OutNote = FText::FromString("Successfully removed stacks");
    return true;
}

TArray<AActor*> UAGR_InventoryManager::GetAllItems()
{
    TArray<AActor*> Items;

    SetupInventoryStorageReference();
    if(!IsValid(InventoryStorage))
    {
        return Items;
    }

    InventoryStorage->GetAttachedActors(Items, true);

    /* Reverse loop as if we do normal loop.
     * By deleting an index, the next item immediately takes its place so we skip one item
     */
    for(int32 i = Items.Num() - 1; i >= 0; --i)
    {
        const AActor* ItemActor = Items[i];

        if(!ItemActor->ActorHasTag(UAGR_ItemComponent::TAG_ITEM))
        {
            Items.RemoveAt(i);
            continue;
        }

        /* Also remove all items from other inventories (yes you can have multiple inventories!) */

        const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(ItemActor);
        if(!IsValid(ItemComponent))
        {
            continue;
        }

        if(ItemComponent->InventoryId != InventoryId)
        {
            Items.RemoveAt(i);
        }
    }

    return Items;
}

bool UAGR_InventoryManager::GetAllItemsOfClass(const TSubclassOf<AActor> Class, TArray<AActor*>& OutFilteredArray)
{
    TArray<AActor*> FilteredArray;
    UKismetArrayLibrary::FilterArray(GetAllItems(), Class, FilteredArray);
    if(FilteredArray.Num() > 0)
    {
        OutFilteredArray = FilteredArray;
        return true;
    }

    return false;
}

bool UAGR_InventoryManager::HasEnoughItems(
    const TSubclassOf<AActor> Item,
    const int32 Quantity,
    UPARAM(DisplayName = "Note") FText& OutNote)
{
    /* Do this check before crafting to see if reduce stack will succeed */

    if(Quantity <= 0)
    {
        OutNote = FText::FromString("Not enough Items. Quantity must be greater than zero");
        return false;
    }

    int32 QuantityMissing = Quantity;

    TArray<AActor*> FilteredArray;
    if(!GetAllItemsOfClass(Item, FilteredArray))
    {
        OutNote = FText::FromString("Has enough check failed: No items of such class found");
        return false;
    }

    for(int32 i = 0; i < FilteredArray.Num(); ++i)
    {
        const AActor* ItemActor = FilteredArray[i];

        const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(ItemActor);
        if(!ensure(IsValid(ItemComponent)))
        {
            continue;
        }

        /* Reduce quantity seeking */
        QuantityMissing -= ItemComponent->CurrentStack;

        if(QuantityMissing <= 0)
        {
            /* Got enough */
            break;
        }
    }

    if(QuantityMissing <= 0)
    {
        OutNote = FText::FromString("Success! Got enough items");
        return true;
    }

    OutNote = FText::FromString("Failed. Not enough items");
    return false;
}

bool UAGR_InventoryManager::GetAllItemsOfTagSlotType(
    const FGameplayTag SlotTypeFilter,
    UPARAM(DisplayName = "ItemsWithTag") TArray<AActor*>& OutItemsWithTag)
{
    TArray<AActor*> ItemsOfSlot;

    TArray<AActor*> AllItems = GetAllItems();
    for(AActor* ItemActor : AllItems)
    {
        const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(ItemActor);
        if(!IsValid(ItemComponent))
        {
            continue;
        }

        if(ItemComponent->ItemTagSlotType.MatchesTag(SlotTypeFilter))
        {
            ItemsOfSlot.Add(ItemActor);
        }
    }

    if(ItemsOfSlot.Num() > 0)
    {
        OutItemsWithTag = ItemsOfSlot;
        return true;
    }

    return false;
}

void UAGR_InventoryManager::AddItemToInventoryDirectly(AActor* Item)
{
    AActor* InventoryManagerOwner = GetOwner();
    if(!IsValid(InventoryManagerOwner) || !InventoryManagerOwner->HasAuthority())
    {
        return;
    }

    /* Is an item? */
    if(!IsValid(Item) || !Item->ActorHasTag(UAGR_ItemComponent::TAG_ITEM))
    {
        return;
    }

    UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(Item);
    if(!IsValid(ItemComponent))
    {
        return;
    }

    /* Not already in inventory */
    if(ItemComponent->InventoryId == InventoryId)
    {
        return;
    }

    ItemComponent->HideShowItem(true);

    /* Ownership to call functions */

    ItemComponent->InventoryId = InventoryId;
    Item->SetOwner(InventoryManagerOwner);
    Item->SetInstigator(InventoryManagerOwner->GetInstigator());

    /* Move to inventory */

    SetupInventoryStorageReference();
    if(!IsValid(InventoryStorage))
    {
        return;
    }

    const FAttachmentTransformRules AttachmentRules(
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::KeepWorld,
        false);
    Item->AttachToActor(InventoryStorage, AttachmentRules, NAME_None);

    /* Set owner if it has no owner */
    if(!ItemComponent->OwnerId.IsValid())
    {
        ItemComponent->OwnerId = InventoryId;
    }

    ItemComponent->OnPickup.Broadcast(this);
    OnItemUpdated.Broadcast(Item);
}

bool UAGR_InventoryManager::HasExactItem(AActor* Item)
{
    return GetAllItems().Contains(Item);
}