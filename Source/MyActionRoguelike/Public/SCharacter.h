// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractionComponent.h"
#include "SProjectileBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USInteractionComponent;
class UAnimMontage;
class USAttributeComponent;

UCLASS()
class MYACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName TimeToHitParamName;
	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName HandSocketName;
	
	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<ASProjectileBase> ProjectileClass;
	UPROPERTY(EditAnywhere, Category= "Attack")
	TSubclassOf<ASProjectileBase> BlackHoleClass;
	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<ASProjectileBase> DashClass;
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;
	UPROPERTY(EditAnywhere, Category="Attack")
	TObjectPtr<UParticleSystem> MuzzleFlashEffect;

	FTimerHandle TimerHandle_PrimaryAttack;
	FTimerHandle TimerHandle_SecondaryAttack;
	FTimerHandle TimerHandle_DashAbility;

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere)
	USInteractionComponent* InteractionComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Componets")
	USAttributeComponent* AttributeComp;
	UPROPERTY(EditAnywhere, Category="Hit")
	float HitFadeSpeed = 4.0f;
	UPROPERTY(EditAnywhere, Category="Hit")
	FColor FadeColour = FColor::Red;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void PrimaryAttack();
	void PrimaryAttack_Elapsed();
	void SecondaryAttack();
	void SecondaryAttack_Elapsed();
	void DashAbility();
	void DashAbility_Elapsed();
	void PrimaryInteract();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent*
						 OwningComp, float NewHealth, float Delta);

	virtual void PostInitializeComponents() override;

private:
	void SpawnProjectile(TSubclassOf<AActor> ClassToSpawn);
	//void GetProjectileSpawnTransform(FTransform& SpawnTM) const;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
