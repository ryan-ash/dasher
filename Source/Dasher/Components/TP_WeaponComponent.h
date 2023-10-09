// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_WeaponComponent.generated.h"

class ADasherCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DASHER_API UTP_WeaponComponent : public USkeletalMeshComponent
{
    GENERATED_BODY()

public:
    /** Projectile class to spawn */
    UPROPERTY(EditDefaultsOnly, Category=Projectile)
    TSubclassOf<class ADasherProjectile> ProjectileClass;

    /** Sound to play each time we fire */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
    USoundBase* FireSound;
    
    /** AnimMontage to play each time we fire */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    UAnimMontage* FireAnimation;

    /** Gun muzzle's offset from the characters location */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
    FVector MuzzleOffset;

    /** Sets default values for this component's properties */
    UTP_WeaponComponent();

    /** Attaches the actor to a FirstPersonCharacter */
    UFUNCTION(BlueprintCallable, Category="Weapon")
    void AttachWeapon(ADasherCharacter* TargetCharacter, bool IsFirstPerson);

    /** Make the weapon Fire a Projectile */
    UFUNCTION(BlueprintCallable, Category="Weapon")
    void Fire();

    /** Make the weapon Fire a Projectile on the server */
    UFUNCTION(Server, Reliable)
    void ServerFire();

private:
    /** The Character holding this weapon*/
    ADasherCharacter* Character;
};
