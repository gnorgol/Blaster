// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletShells.h"

// Sets default values
ABulletShells::ABulletShells()
{

	PrimaryActorTick.bCanEverTick = false;

	BulletShellsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletShellsMesh"));
	SetRootComponent(BulletShellsMesh);

}


void ABulletShells::BeginPlay()
{
	Super::BeginPlay();
	
}



