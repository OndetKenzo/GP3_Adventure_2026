// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	SetCanBeDamaged(true);
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(FName("HealthSystem"));
	HealthSystem->AddToRoot();
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}


