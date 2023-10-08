// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

#include "Components/TP_WeaponComponent.h"

#include "DasherCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickedActorUp, AActor*, PickedUpActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAttached, UTP_WeaponComponent*, NewWeaponComponent);

UENUM(BlueprintType)
enum class EMovementSpeed : uint8
{
	None,
	Walk,
	Sprint
};

UCLASS(config=Game)
class ADasherCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	
public:
	ADasherCharacter();

protected:
	virtual void BeginPlay();

public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Character movement speeds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speeds)
	TMap<EMovementSpeed, float> Speeds;

	UPROPERTY(BlueprintAssignable, Category = "Pickup")
	FOnPickedActorUp OnPickedActorUp;

	UPROPERTY(BlueprintAssignable, Category = "Pickup")
	FOnWeaponAttached OnAttachedWeapon;

protected:
	/** Called for movement input */
	UFUNCTION(BlueprintCallable, Category = Input)
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	UFUNCTION(BlueprintCallable, Category = Input)
	void Look(const FInputActionValue& Value);

	/** Called for sprinting input */
	UFUNCTION(BlueprintCallable, Category = Input)
	void Sprint(const FInputActionValue& Value);

	/** Called for stopping sprint input */
	UFUNCTION(BlueprintCallable, Category = Input)
	void StopSprinting(const FInputActionValue& Value);

	/** Called for crouching input */
	UFUNCTION(BlueprintCallable, Category = Input)
	void TryCrouch(const FInputActionValue& Value);

	/** Called for stopping crouch input */
	UFUNCTION(BlueprintCallable, Category = Input)
	void TryUnCrouch(const FInputActionValue& Value);

public:

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	/** Called for picking up actors */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void PickUp(AActor* PickedUpActor);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};
