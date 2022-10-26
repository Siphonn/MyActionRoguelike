// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "Sound/SoundCue.h"
#include "SMagicProjectile.generated.h"

UCLASS()
class MYACTIONROGUELIKE_API ASMagicProjectile : public ASProjectileBase
{
	GENERATED_BODY()

public:
	ASMagicProjectile();

	UPROPERTY(EditAnywhere, Category="Camera Shake")
	TSubclassOf<UCameraShakeBase> CameraShake;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                        FVector NormalImpulse, const FHitResult& Hit) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Sounds")
	USoundCue* ImpactSFX;
	UPROPERTY(EditAnywhere, Category="Magic Projectile")
	float DamageAmount = 20.0f;
};
