// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUp_Action.h"
#include "SAction.h"
#include "SActionComponent.h"


void ASPowerUp_Action::Interact_Implementation(APawn* InstigatorPawn)
{
	//Super::Interact_Implementation(InstigatorPawn);

	// TODO: confirm the Instigator does not have the "ActionToGrant" already
	// if it does not add "ActionToGrant" to there "DefaultAction"

	if (ensure(ActionToGrant))
	{
		USActionComponent* ActionComp = Cast<USActionComponent>(InstigatorPawn->GetComponentByClass(USActionComponent::StaticClass()));
		if (ActionComp)
		{
			// Check if "ActionToGrant" is already known
			if (ActionComp->GetAction(ActionToGrant))
			{
				FString FailMessage = FString::Printf(TEXT("%s is an Action that is already known"), *GetNameSafe(ActionToGrant));
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailMessage);
				return;
			}

			// Add "ActionToGrant" to Actions
			ActionComp->AddAction(InstigatorPawn, ActionToGrant);
			HideAndCooldownPowerUp();
		}
	}
}
