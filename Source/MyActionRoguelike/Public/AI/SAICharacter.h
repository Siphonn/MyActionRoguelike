// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAttributeComponent.h"
#include "SWorldUserWidget.h"
#include "SAICharacter.generated.h"

class USActionComponent;
class UPawnSensingComponent;
class USWorldUserWidget;
class UUserWidget;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USActionComponent* ActionComp;

	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName TimeToHitParam;
	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName TargetActorKey;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category="UI")
	USWorldUserWidget* ActiveHealthBar;
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<USWorldUserWidget> ExclamationMarkWidget;

	UFUNCTION(BlueprintCallable, Category="AI")
	void SetTargetActor(AActor* NewTarget);
	UFUNCTION(BlueprintCallable, Category="AI")
	AActor* GetTargetActor() const;
	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnPawnSeen();
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);
};
