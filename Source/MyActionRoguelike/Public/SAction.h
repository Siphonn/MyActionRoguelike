// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "SAction.generated.h"

class UTexture2D;
class USActionComponent;
class UWorld;

USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bIsRunning;
	UPROPERTY()
	AActor* Instigator;
};

UCLASS(Blueprintable)
class MYACTIONROGUELIKE_API USAction : public UObject
{
	GENERATED_BODY()

public:
	/* Start immediately when added to an action component */
	UPROPERTY(EditDefaultsOnly, Category="Action")
	bool bAutoStart;
	/* Action nickname to start/stop without a reference to the object*/
	UPROPERTY(EditDefaultsOnly, Category="Action")
	FName ActionName;

	void Initialize(USActionComponent* NewActionComp);

	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StartAction(AActor* Instigator);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Action")
	void StopAction(AActor* Instigator);
	UFUNCTION(BlueprintNativeEvent, Category="Action")
	bool CanStart(AActor* Instigator);
	UFUNCTION(BlueprintCallable, Category="Action")
	bool IsRunning() const;

	virtual UWorld* GetWorld() const override;

	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}

protected:
	UPROPERTY(Replicated)
	USActionComponent* ActionComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSoftObjectPtr<UTexture2D> Icon;

	/* Tags added to owning actor when activated, removed when action stops */
	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer GrantsTags;
	/* Action can only start if OwningActor has none of these Tags applied */
	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer BlockedTags;

	UPROPERTY(ReplicatedUsing="OnRep_RepData")
	FActionRepData RepData;

	UPROPERTY(Replicated)
	float TimeStarted;

	UFUNCTION()
	void OnRep_RepData();

	UFUNCTION(BlueprintCallable, Category="Action")
	USActionComponent* GetOwningComponent() const;
};
