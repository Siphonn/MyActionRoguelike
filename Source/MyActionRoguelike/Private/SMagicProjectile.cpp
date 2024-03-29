// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"
#include "SActionComponent.h"
#include "SActionEffect.h"
#include "SAttributeComponent.h"
#include "SGameplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


ASMagicProjectile::ASMagicProjectile()
{
	SphereComp->SetSphereRadius(20.0f);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);

	InitialLifeSpan = 10.0f;
	
	DamageAmount = 20.0f;
}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator())
	{
		//static FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Status.Parry");

		USActionComponent* ActionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));
		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			MovementComp->Velocity = -MovementComp->Velocity;

			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}

		// Apply damage & impulse
		if (USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			Explode();

			//Burning Effect
			if (ActionComp && BurningActionClass && HasAuthority()) // && !ActionComp->ActiveGameplayTags.HasTag(BurningActionClass) ///  && ensure(BurningActionClass)
			{
				ActionComp->AddAction(GetInstigator(), BurningActionClass);
			}
		}
	}
}
