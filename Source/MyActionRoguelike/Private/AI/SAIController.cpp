// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("Behaviour Tree is nullptr! Please assign BehaviourTree in your AI Controller.")))
	{
		RunBehaviorTree(BehaviorTree);
	}
}


/*
//----------------OLD CODE--------------//
/// REMOVED. OnPawnSeen method used in AICharacter to previde player pawn
// APawn* MyPawn = UGameplayStatics::GetPlayerPawn(this, 0);
//
// if(MyPawn)
// {
// 	GetBlackboardComponent()->SetValueAsVector("MoveToLocation", MyPawn->GetActorLocation());
//
// 	GetBlackboardComponent()->SetValueAsObject("TargetActor", MyPawn);
// }
*/
