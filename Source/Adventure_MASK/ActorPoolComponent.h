// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ActorPoolComponent.generated.h"

/**
 * 
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class ADVENTURE_MASK_API UActorPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UActorPoolComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable)
	AActor* Pull();
	
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> PullArray(int32 Count);
	
	UFUNCTION(BlueprintCallable)
	void Push(AActor* Actor);
	
	UFUNCTION(BlueprintCallable)
	void PushArray(TArray<AActor*> Actors);

protected:	
	// The Actor class Prefab that this pool will spawn and manage.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ActorClass;

	// The number of actors that this pool will spawn on begin play.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 DefaultInstanceCount = 25;

	// The maximum number of actors that this pool will contain.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxPoolCount = 50;

	// The number of actors that will be spawned when the pool is empty and a pull request is made.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ExpendPoolCount = 10;

private:
	UPROPERTY()
	TArray<AActor*> AvailableActors;
};
