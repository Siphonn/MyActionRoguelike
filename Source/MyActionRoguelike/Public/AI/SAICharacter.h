// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAttributeComponent.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

class UPawnSensingComponent;

UCLASS()
class MYACTIONROGUELIKE_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASAICharacter();

protected:
	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UPawnSensingComponent* PawnSensingComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USAttributeComponent* AttributeComp;
	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName TimeToHitParam;

	UFUNCTION()
	void SetTargetActor(AActor* NewTarget);
	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent*
						 OwningComp, float NewHealth, float Delta);
};
