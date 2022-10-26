// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"
#include "SAttributeComponent.h"
#include "Kismet/GameplayStatics.h"


ASMagicProjectile::ASMagicProjectile()
{
}

void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
	//SphereComp->OnComponentHit.AddDynamic(this, &ASMagicProjectile::OnActorHit);
}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator())
	{
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(
			OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (AttributeComp)
		{
			AttributeComp->ApplyHealthChange(-DamageAmount);
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSFX, GetActorLocation());

			Destroy();
		}
	}
}

void ASMagicProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != GetInstigator())
	{
		if (ensure(ImpactVFX) && ensure(ImpactSFX))
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, GetActorLocation());
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSFX, GetActorLocation());
			UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraShake, Hit.Location, 500.0f, 1500.0f, 1.0f, false);

			Destroy();
		}
	}
}
