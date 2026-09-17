// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSubsystem.h"
#include "CrowdAgentComponent.h"
#include "Adventure_MASK/ActorPoolComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include <limits>

void UCrowdSubsystem::RegisterComponent(UCrowdAgentComponent* Component)
{
	FTrackedComponent* Duplicate = CrowdAgentComponents.FindByPredicate([Component](const FTrackedComponent& Comp) { return Comp.Component == Component; });
	if (!Duplicate)
	{
		CrowdAgentComponents.Add({ Component, Component->GetOwner()->GetActorLocation() });
		Grid->InsertPoint(Component, Component->GetOwner()->GetActorLocation());
	}
}

void UCrowdSubsystem::UnregisterComponent(UCrowdAgentComponent* Component)
{
	int32 Index = CrowdAgentComponents.IndexOfByPredicate([Component](const FTrackedComponent& Comp) { return Comp.Component == Component; });
	if (Index != INDEX_NONE)
	{
		Grid->RemovePoint(CrowdAgentComponents[Index].Component, CrowdAgentComponents[Index].GridLocation);
		CrowdAgentComponents.RemoveAtSwap(Index);
	}
}

void UCrowdSubsystem::UpdateGrid()
{
	for (int i = 0; i < CrowdAgentComponents.Num(); ++i)
	{
		Grid->RemovePoint(CrowdAgentComponents[i].Component, CrowdAgentComponents[i].GridLocation);
		CrowdAgentComponents[i].GridLocation = CrowdAgentComponents[i].Component->GetOwner()->GetActorLocation();
		Grid->InsertPoint(CrowdAgentComponents[i].Component, CrowdAgentComponents[i].GridLocation);
	}
}

void UCrowdSubsystem::FormCircle(FVector Location, float Radius, float Duration, float MinRandomOffset, float MaxRandomOffset)
{
	if (!bShotCircle && BlockedCrowdAgentComponents.Num() > 0)
	{
		return;
	}
	else if (Duration == 0.f && bShotCircle)
	{
		bShotCircle = false;
		BreakCircle();
	}
	else
	{
		BreakCircle();
	}

	TArray<UCrowdAgentComponent*> Agents;
	Grid->FindPointsInBall(Location, Radius, [this, Location](UCrowdAgentComponent* Agent) -> double
		{
			if(!Agent || !Agent->GetOwner())
			{
				UE_LOG(LogTemp, Warning, TEXT("No agent or owner"));
				return std::numeric_limits<float>::infinity();
			}

			return FVector::DistSquared(Location, Agent->GetOwner()->GetActorLocation());
		}, Agents);

	for (int i = 0; i < Agents.Num(); ++i)
	{
		// Skip Player and Elites
		if (!Agents[i]->bAvoid)
			continue;

		FVector ActorLocation = Agents[i]->GetOwner()->GetActorLocation();
		FVector FleeVector = ActorLocation - Location;
		FleeVector.Z = 0.f;
		if (FleeVector.IsNearlyZero())
			FleeVector = Agents[i]->GetOwner()->GetActorForwardVector();
		float FleeStrength = 1.f - FleeVector.Length() / Radius;
		FleeVector.Normalize();

		// If circle formed by a shot or else the start of a combat
		if (Duration > 0.f)
		{
			Agents[i]->Lock();
			Agents[i]->MoveTo(ActorLocation + FleeVector * FleeStrength * Radius * FMath::FRandRange(MinRandomOffset, MaxRandomOffset));
			Agents[i]->LookAt(ActorLocation + FleeVector);
			CircleBreakTime = GetWorld()->GetTime().GetWorldTimeSeconds() + Duration;
			bShotCircle = true;
		}
		else
		{
			Agents[i]->SetAvoidance(false);
			Agents[i]->MoveTo(ActorLocation + FleeVector * FleeStrength * Radius);
			Agents[i]->LookAt(Location);
		}

		BlockedCrowdAgentComponents.Add(Agents[i]);
	}
}

void UCrowdSubsystem::BreakCircle()
{
	for (int i = 0; i < BlockedCrowdAgentComponents.Num(); ++i)
	{
		BlockedCrowdAgentComponents[i]->SetAvoidance(true);
		BlockedCrowdAgentComponents[i]->Unlock();
	}

	BlockedCrowdAgentComponents.Empty();
}

void UCrowdSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float Time = GetWorld()->GetTime().GetWorldTimeSeconds();

	if (Time >= GridLastUpdateTime + GridUpdateRate)
	{
		GridLastUpdateTime = Time;
		UpdateGrid();
	}

	if (bShotCircle)
	{
		if (Time >= CircleBreakTime)
		{
			bShotCircle = false;
			BreakCircle();
		}
	}
}

TStatId UCrowdSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCrowdSubsystem, STATGROUP_Tickables);
}

void UCrowdSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Grid = new UE::Geometry::TPointHashGrid3d<UCrowdAgentComponent*>(300, nullptr);
}

void UCrowdSubsystem::Deinitialize()
{
	Super::Deinitialize();

	delete Grid;
}