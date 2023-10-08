// Copyright Epic Games, Inc. All Rights Reserved.

#include "DasherCharacter.h"

#include "Actors/DasherProjectile.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// ADasherCharacter

ADasherCharacter::ADasherCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void ADasherCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

void ADasherCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Remove Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(DefaultMappingContext);
		}
	}
	UnsubscribeToWeaponInput();

	Super::EndPlay(EndPlayReason);
}

//////////////////////////////////////////////////////////////////////////// Input

void ADasherCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADasherCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADasherCharacter::Look);

		//Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ADasherCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ADasherCharacter::StopSprinting);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ADasherCharacter::TryCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ADasherCharacter::TryUnCrouch);
	}
}


void ADasherCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ADasherCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADasherCharacter::Sprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->IsCrouching() || GetCharacterMovement()->IsFalling() || !Speeds.Contains(EMovementSpeed::Sprint))
	{
		return;
	}
	GetCharacterMovement()->MaxWalkSpeed = Speeds[EMovementSpeed::Sprint];
}

void ADasherCharacter::StopSprinting(const FInputActionValue& Value)
{
	if (!Speeds.Contains(EMovementSpeed::Walk))
	{
		return;
	}
	GetCharacterMovement()->MaxWalkSpeed = Speeds[EMovementSpeed::Walk];
}

void ADasherCharacter::TryCrouch(const FInputActionValue& Value)
{
	Crouch(false);
}

void ADasherCharacter::TryUnCrouch(const FInputActionValue& Value)
{
	UnCrouch(false);
}

void ADasherCharacter::Fire(const FInputActionValue& Value)
{
	if (ActiveWeaponComponent.IsValid())
	{
		ActiveWeaponComponent->Fire();
		ServerFire();
	}
}

void ADasherCharacter::ServerFire_Implementation()
{
	ActiveWeaponComponent->ServerFire();
}

void ADasherCharacter::StopFire(const FInputActionValue& Value)
{

}

void ADasherCharacter::AltFire(const FInputActionValue& Value)
{

}

void ADasherCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
	if (bHasRifle)
	{
		SubscribeToWeaponInput();
	}
}

bool ADasherCharacter::GetHasRifle()
{
	return bHasRifle;
}

void ADasherCharacter::SubscribeToWeaponInput()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FireMappingContext, 1);

			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
			{
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ADasherCharacter::Fire);
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ADasherCharacter::StopFire);
			}
		}
	}
}

void ADasherCharacter::UnsubscribeToWeaponInput()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void ADasherCharacter::PickUp(AActor* PickedUpActor)
{
	OnPickedActorUp.Broadcast(PickedUpActor);
	if (const auto WeaponComponent = PickedUpActor->GetComponentByClass<UTP_WeaponComponent>())
	{
		ActiveWeaponComponent = WeaponComponent;
		OnAttachedWeapon.Broadcast(WeaponComponent);
		WeaponComponent->AttachWeapon(this, IsLocallyControlled());
	}
	PickedUpActor->SetOwner(this);
}
