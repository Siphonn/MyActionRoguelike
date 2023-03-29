// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SActionComponent.generated.h"

class USAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionStateChanged, USActionComponent*, OwningComp, USAction*, Action);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYACTIONROGUELIKE_API USActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tags")
	FGameplayTagContainer ActiveGameplayTags;

	// Sets default values for this component's properties
	USActionComponent();

	UFUNCTION(BlueprintCallable, Category="Action")
	void AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass);
	UFUNCTION(BlueprintCallable, Category="Action")
	void RemoveAction(USAction* ActionToRemove);
	UFUNCTION(BlueprintCallable, Category="Action")
	bool StartActionByName(AActor* Instigator, FName ActionName);
	UFUNCTION(BlueprintCallable, Category="Action")
	bool StopActionByName(AActor* Instigator, FName ActionName);
	UFUNCTION(BlueprintCallable, Category="Action")
	USAction* GetAction(TSubclassOf<USAction> ActionClass) const;

protected:
	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<USAction*> Actions;

	/* Grant abilities at game start */
	UPROPERTY(EditAnywhere, Category="Action")
	TArray<TSubclassOf<USAction>> DefaultActions;
	
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerStartAction(AActor* Instigator, FName ActionName);
	UFUNCTION(Server, Reliable)
	void ServerStopAction(AActor* Instigator, FName ActionName);


public:
	UPROPERTY(BlueprintAssignable)
	FOnActionStateChanged OnActionStarted;
	UPROPERTY(BlueprintAssignable)
	FOnActionStateChanged OnActionStopped;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
};
