// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_WeaponComponent.h"

#include "Characters/DasherCharacter.h"
#include "Actors/DasherProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
    // Default offset from the character location for projectiles to spawn
    MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
    if (Character == nullptr || Character->GetController() == nullptr)
    {
        return;
    }

    // Try and play the sound if specified
    if (FireSound != nullptr)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
    }
    
    // Try and play a firing animation if specified
    if (FireAnimation != nullptr)
    {
        // Get the animation object for the arms mesh
        UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
        if (AnimInstance != nullptr)
        {
            AnimInstance->Montage_Play(FireAnimation, 1.f);
        }
    }
}

// weapon doesn't know about client & server, we'll control that from the character
void UTP_WeaponComponent::ServerFire_Implementation()
{
    if (Character == nullptr || Character->GetController() == nullptr)
    {
        return;
    }

    // Try and fire a projectile
    if (ProjectileClass != nullptr)
    {
        UWorld* const World = GetWorld();
        if (World != nullptr)
        {
            APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
            const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
            // MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
            const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
    
            //Set Spawn Collision Handling Override
            FActorSpawnParameters ActorSpawnParams;
            ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
    
            // Spawn the projectile at the muzzle
            World->SpawnActor<ADasherProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
        }
    }
}

void UTP_WeaponComponent::AttachWeapon(ADasherCharacter* TargetCharacter, bool IsFirstPerson)
{
    Character = TargetCharacter;
    if (Character == nullptr)
    {
        return;
    }

    if (IsFirstPerson)
    {
        // Attach the weapon to the First Person Character
        FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
        AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
    }
    
    // switch character into gun mode
    Character->SetHasRifle(true);
}
