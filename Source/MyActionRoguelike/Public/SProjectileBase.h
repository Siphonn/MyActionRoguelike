// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SProjectileBase.generated.h"

class UCameraShakeBase;
class USoundCue;
class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;

UCLASS(Abstract)
/// 'ABSTRACT' marks the class as incomplete, keeping this out of certain dropdown windows like SpawnActor in Unreal
class MYACTIONROGUELIKE_API ASProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASProjectileBase();

protected:
	UPROPERTY(EditAnywhere, Category="Effects|Camera Shake")
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Camera Shake")
	float ImpactShakeInnerRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Camera Shake")
	float ImpactShakeOuterRadius;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	UParticleSystem* ImpactVFX;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	USoundCue* ImpactSFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UParticleSystemComponent* EffectComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UAudioComponent* AudioComp;

	/// 'virtual' so we can override this child-class 
	UFUNCTION()
	virtual void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/// BlueprintNativeEvent = C++ base implementation, can be expanded in Blueprints
	/// BlueprintCallable to allow child classes to trigger	explosions
	/// Not required for assignment, useful for expanding in Blueprint later on
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Explode();

	virtual void PostInitializeComponents() override;
};
