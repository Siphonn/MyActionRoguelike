// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USAttributeComponent();

	UFUNCTION(BlueprintCallable, Category="Attributes")
	static USAttributeComponent* GetAttributes(AActor* FromActor);
	UFUNCTION(BlueprintCallable, Category="Attributes", meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor* Actor);

protected:
	/// EditAnywhere - edit in BP editor and per-instance in level.
	/// VisibleAnywhere - 'read-only' in editor and level. (Use for Components)
	/// EditDefaultOnly - hide variable per-instance
	/// VisibleDefaultOnly - 'read-only' access for variable, only in BP editor (uncommon)
	/// EditInstanceOnly - allow only editing of instance (eg. when placed in level)
	/// --
	/// BlueprintReadOnly - read-only in the Blueprint scripting (does not affect 'details' panel)
	/// BlueprintReadWrite - read-write access in Blueprints
	/// --
	/// Category = "" -  displayed only for detail panel and blueprint context menu.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float MaxHealth;

	// MaxHealth, Stamina, Strength


public:
	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;
	UFUNCTION()
	bool IsFullHealth() const;
	UFUNCTION()
	float GetMaxHealth() const;
	UFUNCTION()
	float GetHealth() const;
	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);
};
