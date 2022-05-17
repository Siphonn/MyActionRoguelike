// Fill out your copyright notice in the Description page of Project Settings.


#include "SDashProjectile.h"

#include "SCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

ASDashProjectile::ASDashProjectile()
{
	ExplodeEffectComp = CreateDefaultSubobject<UParticleSystemComponent>("ExplodeEffect");
	ExplodeEffectComp->SetupAttachment(SphereComp);
	ExplodeEffectComp->SetAutoActivate(false);
}

void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	InstigatorPawn = GetInstigator();

	SphereComp->IgnoreActorWhenMoving(InstigatorPawn, true);

	GetWorldTimerManager().SetTimer(TimerHandle_Teleport, this, &ASDashProjectile::DashExplosion_Delay, 0.5f);
}

void ASDashProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
                                 bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
                                 const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	//UE_LOG(LogTemp, Warning, TEXT("HIT!!!"));
	DashExplosion_Delay();
}

void ASDashProjectile::DashExplosion_Delay()
{
	EffectComp->Deactivate();
	ExplodeEffectComp->Activate();
	MovementComp->Velocity = FVector(0.0f, 0.0f, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_Teleport, this, &ASDashProjectile::Teleport_Delay, 0.2f);
}

void ASDashProjectile::Teleport_Delay()
{
	InstigatorPawn->TeleportTo(GetActorLocation(), InstigatorPawn->GetActorRotation(), true, false);
	ASCharacter* PlayerCharacter = Cast<ASCharacter>(InstigatorPawn);
	PlayerCharacter->GetMovementComponent()->Velocity = FVector(0, 0, 0);
	PlayerCharacter->Jump();

	Destroy();
}
