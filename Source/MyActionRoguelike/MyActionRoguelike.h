// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

static void LogToScreen(UObject* WorldContext, FString Msg, FColor Colour = FColor::White, float Duration = 5.0f)
{
	if (!ensure(WorldContext)) { return; }

	UWorld* World = WorldContext->GetWorld();
	if (!ensure(World)) { return; }

	FString NetPrefix = World->IsNetMode(NM_Client) ? "[CLIENT] " : "[SERVER] ";
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Colour, NetPrefix + Msg);
	}
}
