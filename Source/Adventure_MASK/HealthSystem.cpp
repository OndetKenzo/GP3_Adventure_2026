// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthSystem.h"

#include "GameFramework/Character.h"
#include "Misc/MapErrors.h"


// Sets default values for this component's properties
UHealthSystem::UHealthSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UHealthSystem::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
	
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		if (bBindReceiveAnyDamage)
			OwnerCharacter->OnTakeAnyDamage.AddDynamic(this, &UHealthSystem::ReceiveAnyDamage);
		
		if (bBindReceivePointDamage)
			OwnerCharacter->OnTakePointDamage.AddDynamic(this, &UHealthSystem::ReceivePointDamage);
		
		if (bBindReceiveRadialDamage)
			OwnerCharacter->OnTakeRadialDamage.AddDynamic(this, &UHealthSystem::ReceiveRadialDamage);
		
	}
}

void UHealthSystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (OwnerCharacter)
	{
		if (bBindReceiveAnyDamage)
			OwnerCharacter->OnTakeAnyDamage.RemoveDynamic(this, &UHealthSystem::ReceiveAnyDamage);
		
		if (bBindReceivePointDamage)
			OwnerCharacter->OnTakePointDamage.RemoveDynamic(this, &UHealthSystem::ReceivePointDamage);
		
		if (bBindReceiveRadialDamage)
			OwnerCharacter->OnTakeRadialDamage.RemoveDynamic(this, &UHealthSystem::ReceiveRadialDamage);
		
	}
}

void UHealthSystem::ReceiveAnyDamage_Implementation(AActor* DamagedActor, float Damage,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	ProcessDamage(Damage);
}

void UHealthSystem::ReceivePointDamage_Implementation(AActor* DamagedActor, float Damage,
	class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName,
	FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	ProcessDamage(Damage);
}

void UHealthSystem::ReceiveRadialDamage_Implementation(AActor* DamagedActor, float Damage,
	const class UDamageType* DamageType, FVector Origin, const FHitResult& HitInfo, class AController* InstigatedBy,
	AActor* DamageCauser)
{
	ProcessDamage(Damage);
}

void UHealthSystem::ProcessDamage(float DamageAmount)
{
	CurrentHealth -= DamageAmount;
	if (CurrentHealth <= 0)
	{
		CurrentHealth = 0;
		bIsDead = true;
	}
}
