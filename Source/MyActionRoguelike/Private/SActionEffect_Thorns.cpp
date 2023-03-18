// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect_Thorns.h"

#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SGameplayFunctionLibrary.h"

USActionEffect_Thorns::USActionEffect_Thorns()
{
	// TODO: Deal fraction of damage back to attacker
	// Round reflected damage to nearest int (FMath::Round)
	// Infinite duration
	// Don't reflect damage to owner

	Duration = 0.0f; //INFINITY
	Period = 0.0f;

	ReflectFraction = 0.25f;
}

void USActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	// Bind to OnHealthChanged (AddDynamic)
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());
	if (AttributeComp)
	{
		AttributeComp->OnHealthChanged.AddDynamic(this, &USActionEffect_Thorns::OnHealthChanged);
	}
}

void USActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	// Unbind from OnHealthChanged (RemoveDynamic)
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());
	if (AttributeComp)
	{
		AttributeComp->OnHealthChanged.RemoveDynamic(this, &USActionEffect_Thorns::OnHealthChanged);
	}
}

void USActionEffect_Thorns::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	AActor* OwningActor = GetOwningComponent()->GetOwner();

	// Damage only
	if (Delta < 0.0f && OwningActor != InstigatorActor)
	{
		// round to the nearest integer
		int32 ReflectionAmount = FMath::RoundToInt(Delta * ReflectFraction);
		if (ReflectionAmount == 0)
		{
			return;
		}

		// Flip to positive, so we don't heal ourselves when passing into damage
		ReflectionAmount = FMath::Abs(ReflectionAmount);

		// Damage to sender
		USGameplayFunctionLibrary::ApplyDamage(OwningActor, InstigatorActor, ReflectionAmount);
	}
}
