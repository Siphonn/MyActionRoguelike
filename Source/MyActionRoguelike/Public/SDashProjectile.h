// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMagicProjectile.h"
#include "SDashProjectile.generated.h"

UCLASS()
class MYACTIONROGUELIKE_API ASDashProjectile : public ASProjectile
{
	GENERATED_BODY()

public:
	ASDashProjectile();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* ExplodeEffectComp;
	// UPROPERTY(EditAnywhere, Category = "Projectile")
	// UAnimMontage* FallAnim;

	FTimerHandle TimerHandle_Teleport;
	FTimerHandle TimerHandle_Destroy;

	virtual void BeginPlay() override;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void DashExplosion_Delay();
	void Teleport_Delay();

private:
	APawn* InstigatorPawn;
};
