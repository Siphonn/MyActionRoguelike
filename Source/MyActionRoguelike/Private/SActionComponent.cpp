// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "SAction.h"
#include "../MyActionRoguelike.h"

static TAutoConsoleVariable<bool> CVarActiveActionData(TEXT("su.ActiveActionData"), false,TEXT("Show current active Action in the ActionComponent and there active/inactive state."), ECVF_Cheat);

DECLARE_CYCLE_STAT(TEXT("StartActionByName"), STAT_StartActionByName, STATGROUP_STANFORD);

USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Server Only
	if (GetOwner()->HasAuthority())
	{
		for (TSubclassOf<USAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}
}

void USActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Stop all
	TArray<USAction*> ActionCopy = Actions;
	for(USAction* Action:ActionCopy)
	{
		if(Action&& Action->IsRunning())
		{
			Action->StopAction(GetOwner());
		}	
	}
	
	Super::EndPlay(EndPlayReason);
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// FString DebugMessage = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	// GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMessage);

	// Draw All Actions
	if (CVarActiveActionData.GetValueOnGameThread())
	{
		for (USAction* Action : Actions)
		{
			FColor TextColour = Action->IsRunning() ? FColor::Blue : FColor::White;

			FString ActionMsg = FString::Printf(TEXT("[%s] Action is: %s"), *GetNameSafe(GetOwner()), *GetNameSafe(Action));

			LogToScreen(this, ActionMsg, TextColour, 0.0f);
		}
	}
}

void USActionComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass)) { return; }

	// Skip for clients
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client attempting to AddAction. [Class: %s]"), *GetNameSafe(ActionClass));
		return;
	}

	USAction* NewAction = NewObject<USAction>(GetOwner(), ActionClass);
	if (ensure(NewAction))
	{
		NewAction->Initialize(this);

		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
}

void USActionComponent::RemoveAction(USAction* ActionToRemove)
{
	if (ensure(ActionToRemove && !ActionToRemove->IsRunning()))
	{
		return;
	}
	Actions.Remove(ActionToRemove);
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	SCOPE_CYCLE_COUNTER(STAT_StartActionByName);

	for (USAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (!Action->CanStart(Instigator))
			{
				FString FailMessage = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailMessage);
				continue;
			}

			// will run only if is Client
			if (!GetOwner()->HasAuthority())
			{
				// Start Action on Server
				ServerStartAction(Instigator, ActionName);
			}

			// Bookmark for Unreal Insights
			TRACE_BOOKMARK(TEXT("Start Action::%s"), *GetNameSafe(Action));

			// Start Action locally 
			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->IsRunning())
			{
				// Is Client?
				if (!GetOwner()->HasAuthority())
				{
					// Start Action on Server
					ServerStopAction(Instigator, ActionName);
				}

				Action->StopAction(Instigator);
				return true;
			}
		}
	}
	return false;
}

// Returns the same Action if it is in'Actions'
USAction* USActionComponent::GetAction(TSubclassOf<USAction> ActionClass) const
{
	for (USAction* Action : Actions)
	{
		if (Action && Action->IsA(ActionClass))
		{
			return Action;
		}
	}

	return nullptr;
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}

void USActionComponent::ServerStopAction_Implementation(AActor* Instigator, FName ActionName)
{
	StopActionByName(Instigator, ActionName);
}

bool USActionComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (USAction* Action : Actions)
	{
		if (Action)
		{
			WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USActionComponent, Actions);
}
