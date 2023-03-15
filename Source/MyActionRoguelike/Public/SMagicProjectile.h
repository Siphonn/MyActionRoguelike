// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamageAmount;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTag ParryTag;
	UPROPERTY(EditDefaultsOnly, Category="Sounds")
	USoundCue* ImpactSFX;


	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
