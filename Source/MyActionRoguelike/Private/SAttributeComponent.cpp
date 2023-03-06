// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"
#include "SGameModeBase.h"


static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Gloable damage multiplier for Attribute Component."), ECVF_Cheat);


// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = 100;
	MaxHealth = 100;
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	USAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}

	return false;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return Health == MaxHealth;
}

float USAttributeComponent::GetMaxHealth() const
{
	return MaxHealth;
}

float USAttributeComponent::GetHealth() const
{
	return Health;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetMaxHealth());
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		return false;
	}

	if(Delta< 0.0f)
	{
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiplier;
	}

	float OldHealth = Health;

	Health += Delta;
	Health = FMath::Clamp(Health, 0.0f, MaxHealth);

	float ActualDelta = Health - OldHealth;
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	// Died
	if (ActualDelta < 0.0f && Health == 0.0f)
	{
		ASGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if (GameMode)
		{
			GameMode->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}

	return ActualDelta != 0;
}
