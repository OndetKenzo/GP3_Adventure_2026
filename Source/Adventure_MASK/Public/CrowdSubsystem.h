// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Spatial/PointHashGrid3.h"
#include "CrowdSubsystem.generated.h"


class UCrowdAgentComponent;

struct FTrackedComponent
{
	UCrowdAgentComponent* Component;
	FVector GridLocation;
};

/**
 * 
 */
UCLASS()
class ADVENTURE_MASK_API UCrowdSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	

private:
	TArray<FTrackedComponent> CrowdAgentComponents;
	TArray<UCrowdAgentComponent*> BlockedCrowdAgentComponents;

	UE::Geometry::TPointHashGrid3d<UCrowdAgentComponent*>* Grid;
	float GridUpdateRate = 0.5f;
	float GridLastUpdateTime = 0.f;

	float CircleBreakTime = 0.f;
	bool bShotCircle = false;

public:

	// Tickable
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	// Subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	void RegisterComponent(UCrowdAgentComponent* Component);
	void UnregisterComponent(UCrowdAgentComponent* Component);
	
	/// <summary>
	/// Make crowd agents present in radius form a circle
	/// </summary>
	/// <param name="Location">Where to start the circle</param>
	/// <param name="Radius">Size of the circle</param>
	/// <param name="Duration">Duration of the circle, if 0 will last indefinitely</param>
	UFUNCTION(BlueprintCallable)
	void FormCircle(FVector Location, float Radius, float Duration = 0.f, float MinRandomOffset = 1.f, float MaxRandomOffset = 1.f);

	/// <summary>
	/// Only call when circle has infinite duration
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void BreakCircle();

private:

	void UpdateGrid();
};
