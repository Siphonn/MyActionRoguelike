// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SAttributeComponent.h"
#include "SInteractionComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// -- Rotation Visualization -- //
	const float DrawScale = 100.0f;
	const float Thickness = 5.0f;

	FVector LineStart = GetActorLocation();
	// Offset to the right of pawn
	LineStart += GetActorRightVector() * 100.0f;
	// Set line end in direction of the actor's forward
	FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
	// Draw Actor's Direction
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0,
	                          Thickness);

	FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
	// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f,
	                          0, Thickness);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::SecondaryAttack);
	PlayerInputComponent->BindAction("DashAbility", IE_Pressed, this, &ASCharacter::DashAbility);
}

void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	AddMovementInput(ControlRot.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	// X = Forward (Red)
	// Y = Right (Green)
	// Z = Up (Blue)

	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, Value);
}

void ASCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_Elapsed, 0.2f);

	//GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);
}

void ASCharacter::PrimaryAttack_Elapsed()
{
	SpawnProjectile(ProjectileClass);
}

void ASCharacter::SecondaryAttack()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_SecondaryAttack, this, &ASCharacter::SecondaryAttack_Elapsed, 0.2f);
}

void ASCharacter::SecondaryAttack_Elapsed()
{
	SpawnProjectile(BlackHoleClass);
}

void ASCharacter::DashAbility()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_DashAbility, this, &ASCharacter::DashAbility_Elapsed, 0.2f);
}

void ASCharacter::DashAbility_Elapsed()
{
	SpawnProjectile(DashClass);
}

void ASCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn)
{
	if (!ensureAlways(ClassToSpawn)) { return; }

	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	FCollisionShape Shape;
	Shape.SetSphere(20.0f);

	/// Ignore player
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FVector TraceStart = CameraComp->GetComponentLocation();

	/// endpoint far into the look-at distance (not too far, still adjust somewhat towards crosshair on a miss)
	FVector TraceEnd = CameraComp->GetComponentLocation() + (GetControlRotation().Vector() * 5000.0f);

	FHitResult Hit;
	/// returnss true if we got to blocking hit
	if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, Shape,
	                                        Params))
	{
		/// Overwrite trace end with impact point in world
		TraceEnd = Hit.ImpactPoint;
	}

	/// find new direction/rotation from Hand pointing to impact point in world
	FRotator ProjectRotation = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();

	FTransform SpawnTM = FTransform(ProjectRotation, HandLocation);
	GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTM, SpawnParams);
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
	}
}


// void ASCharacter::GetProjectileSpawnTransform(FTransform& SpawnTM) const
// {
// 	FCollisionObjectQueryParams ObjectQueryParams;
// 	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
// 	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
// 	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
//
// 	FVector CameraLocation = CameraComp->GetComponentLocation();;
// 	FRotator CameraRotation = CameraComp->GetComponentRotation();
// 	FVector CameraEnd = CameraLocation + (CameraRotation.Vector() * 10000.0f);
//
// 	FHitResult Hit;
// 	bool bHasHit = GetWorld()->LineTraceSingleByObjectType(Hit, CameraLocation, CameraEnd, ObjectQueryParams);
//
// 	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
// 	FVector EndLocation = (bHasHit ? Hit.Location : CameraEnd);
// 	FRotator FireRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, EndLocation);
//
// 	SpawnTM = FTransform(FireRotation, HandLocation);
// }


/// Debug LineTrace from camera
// 	UE_LOG(LogTemp, Warning, TEXT("LineTrace Hit! %s was the actor Hit!!"), *GetNameSafe(Hit.GetActor()));
// 	DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 30, 32, FColor::Blue, false, 2.0f);


/// UKismetMathLibrary::FindLookAtRotation(HandLocation, EndLocation);
/// OR
/// FVector LocationDiff = EndLocation - HandLocation;
/// FRotator LooKAtRotation = TestLoc.Rotation();

// if (!ensure(BlackHoleClass)) { return; }
//
// FTransform SpawnTM;
// GetProjectileSpawnTransform(SpawnTM);
//
// FActorSpawnParameters SpawnParams;
// SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
// SpawnParams.Instigator = this;
//
// GetWorld()->SpawnActor<AActor>(BlackHoleClass, SpawnTM, SpawnParams);
