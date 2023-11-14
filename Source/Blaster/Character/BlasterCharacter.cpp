// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/BlasterComponents/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlastAnimInstance.h"
#include "Blaster/Blaster.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "TimerManager.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include <Kismet/GameplayStatics.h>
#include "Components/BoxComponent.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"


// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;


	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	ViewCamera->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetRootComponent());

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	BuffComponent->SetIsReplicated(true);

	LagCompensationComponent = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensationComponent"));


	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/**
		Hit Box for server side rewind
	*/
	head = CreateDefaultSubobject<UBoxComponent>(TEXT("Head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("Pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	spine_04 = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine_04"));
	spine_04->SetupAttachment(GetMesh(), FName("spine_04"));
	spine_05 = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine_05"));
	spine_05->SetupAttachment(GetMesh(), FName("spine_05"));
	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("Upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("Upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("Lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("Lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("Hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("Hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("Thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("Thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("Calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("Calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("Foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("Foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));

	HitCollisionBoxes.Add(FName("head"), head);
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);
	HitCollisionBoxes.Add(FName("spine_04"), spine_04);
	HitCollisionBoxes.Add(FName("spine_05"), spine_05);
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		}

	}

}

void ABlasterCharacter::DeathTimerFinished()
{
	ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (BlasterGameMode)
	{
		BlasterGameMode->RequestRespawn(this, Controller);
	}

}

void ABlasterCharacter::KillCam(float DeltaTime)
{
	if (ViewCamera)
	{
		FVector KillerLocation = Killer->GetActorLocation();
		FVector KillerDirection = KillerLocation - ViewCamera->GetComponentLocation();
		FRotator KillerRotation = UKismetMathLibrary::MakeRotFromX(KillerDirection);
		ViewCamera->SetWorldRotation(KillerRotation);
	}
}


void ABlasterCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr)
	{
		return;
	}
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->DropWeapon();
	}
}



void ABlasterCharacter::RagdollDeath()
{
	if (CombatComponent)
	{
		if (CombatComponent->EquippedWeapon)
		{
			DropOrDestroyWeapon(CombatComponent->EquippedWeapon);
		}
		if (CombatComponent->SecondaryWeapon)
		{
			DropOrDestroyWeapon(CombatComponent->SecondaryWeapon);
		}
	}
	MulticastRagdollDeath();
	bIsDead = true;
	GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &ABlasterCharacter::DeathTimerFinished, DeathDelay);
}

void ABlasterCharacter::MulticastRagdollDeath_Implementation()
{

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("pelvis"), 1.0f, true);
	GetMesh()->bBlendPhysics = true;
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetConstraintMode(EDOFMode::Default);

	GetCharacterMovement()->DisableMovement();

	if (BlasterPlayerController)
	{
		//disable Input
		bDisableGameplayInput = true;
		if (CombatComponent)
		{
			CombatComponent->FireButtonPressed(false);
		}
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(BlastCharacterMappingContext);
			}
		}
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AttachedGrenade)
	{
		AttachedGrenade->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GrenadeSocket"));
		AttachedGrenade->SetVisibility(false);
	}
	
	Tags.Add(FName("Player"));
	SpawnDefaultWeapon();
	UpdateHUDAmmo();
	UpdateHUDHealth();
	UpdateHUDShield();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(BlastCharacterMappingContext, 0);
		}
	}
}
void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}
void ABlasterCharacter::UpdateHUDShield()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDShield(Shield, MaxShield);
	}
}
void ABlasterCharacter::UpdateHUDAmmo()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController && CombatComponent && CombatComponent->EquippedWeapon)
	{
		BlasterPlayerController->SetHUDCarriedAmmo(CombatComponent->CarriedAmmo);
		BlasterPlayerController->SetHUDWeaponAmmo(CombatComponent->EquippedWeapon->GetCurrentAmmo());	
	}
}
void ABlasterCharacter::SpawnDefaultWeapon()
{
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if (BlasterGameMode && World && !bIsDead && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (StartingWeapon && CombatComponent)
		{
			CombatComponent->EquipWeapon(StartingWeapon);
		}
	}
}
float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}
void ABlasterCharacter::HideCharacter(bool bHide)
{
	//Get all children of the mesh character
	TArray<USceneComponent*> Childrens;
	GetMesh()->GetChildrenComponents(true, Childrens);
	for (USceneComponent* Child : Childrens)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Child))
		{
			Prim->SetOwnerNoSee(bHide);
		}
	}
}
void ABlasterCharacter::OnRep_Health(float OldHealth)
{
	UpdateHUDHealth();
	if (Health < OldHealth)
	{
		
		PlayHitReactMontage();
	}	
}

void ABlasterCharacter::OnRep_Shield(float OldShield)
{
	UpdateHUDShield();
	if (Shield < OldShield)
	{
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::HideCameraIfCharacterCloseToWall()
{
	if (!IsLocallyControlled())
	{
		return;
	}
	if ((ViewCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		HideCharacter(true);
		if (CombatComponent && CombatComponent-> EquippedWeapon && CombatComponent->EquippedWeapon->GetWeaponMesh())
		{
			CombatComponent->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		HideCharacter(false);
		if (CombatComponent && CombatComponent->EquippedWeapon && CombatComponent->EquippedWeapon->GetWeaponMesh())
		{
			CombatComponent->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ABlasterCharacter::PollInit()
{
	if (BlasterPlayerState == nullptr)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if (BlasterPlayerState)
		{
			BlasterPlayerState->AddToScore(0.f);
			BlasterPlayerState->AddToDefeats(0);
			
		}
	}
}



// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateInPlace(DeltaTime);
	if (Killer)
	{
		KillCam(DeltaTime);
	}

	HideCameraIfCharacterCloseToWall();
	PollInit();
}
void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplayInput)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}
void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (CombatComponent && CombatComponent->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (CombatComponent && CombatComponent->bAiming);
}

void ABlasterCharacter::OnRep_Killer()
{
}


void ABlasterCharacter::SetKiller(ABlasterCharacter* Character)
{
	Killer = Character;
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (CombatComponent == nullptr)
	{
		return nullptr;
	}
	return CombatComponent->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (CombatComponent == nullptr)
	{
		return FVector();
	}
	return CombatComponent->HitTarget;
}

ECombatState ABlasterCharacter::GetCombatState() const
{
	if (CombatComponent == nullptr)
	{
		return ECombatState::ECS_Max;
	}
	return CombatComponent->CombatState;
}

bool ABlasterCharacter::IsLocallyReloading()
{
	if (CombatComponent == nullptr)
	{
		return false;
	}
	return CombatComponent->bLocallyReloading;
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}



void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (CombatComponent)
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}

}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Equip);
		EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::SwitchWeapon);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::CrouchPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::AimPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::FirePressed);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::ReloadPressed);
		EnhancedInputComponent->BindAction(ThrowGrenadeAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::ThrowGrenadePressed);
	}

}
void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComponent)
	{
		CombatComponent->Character = this;
	}
	if (BuffComponent)
	{
		BuffComponent->Character = this;
	}
	if (LagCompensationComponent)
	{
		LagCompensationComponent->Character = this;
		if (Controller)
		{
			LagCompensationComponent->Controller = Cast<ABlasterPlayerController>(Controller);
		}
	}
}
void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr)
	{
		return;
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? TEXT("RifleAim") : TEXT("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}


}
void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void ABlasterCharacter::Destroyed()
{
	Super::Destroyed();

	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = BlasterGameMode && BlasterGameMode->GetMatchState() != MatchState::InProgress;

	if (CombatComponent && CombatComponent->EquippedWeapon && bMatchNotInProgress)
	{
		CombatComponent->EquippedWeapon->Destroy();
	}
}

void ABlasterCharacter::MulticastEliminated_Implementation()
{
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}
	bIsDead = true;
	PlayDeathMontage();
	bool bHideSniperScope = IsLocallyControlled() && CombatComponent && CombatComponent->EquippedWeapon && CombatComponent->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}
}
void ABlasterCharacter::PlayHitReactMontage()
{
	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr)
	{
		return;
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName = TEXT("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}
void ABlasterCharacter::PlayDeathMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		int RandomSection = FMath::RandRange(1, 3);
		FName SectionName = FName(*FString::Printf(TEXT("Death%d"), RandomSection));
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}
void ABlasterCharacter::PlayReloadMontage()
{
	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr)
	{
		return;
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		switch (CombatComponent->EquippedWeapon->GetWeaponType())
		{
			case EWeaponType::EWT_AssaultRifle:
				SectionName = TEXT("Rifle");
				break;
			case EWeaponType::EWT_RocketLauncher:
				SectionName = TEXT("Rifle");
				break;
			case EWeaponType::EWT_Pistol:
				SectionName = TEXT("Pistol");
				break;
			case EWeaponType::EWT_SubmachineGun:
				SectionName = TEXT("Pistol");
				break;
			case EWeaponType::EWT_Shotgun:
				SectionName = TEXT("Shotgun");
				break;
			case EWeaponType::EWT_SniperRifle:
				SectionName = TEXT("Sniper");
				break;
			case EWeaponType::EWT_GrenadeLauncher:
				SectionName = TEXT("GrenadeLauncher");
				break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}
void ABlasterCharacter::PlayThrowGrenadeMontage()
{
	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr)
	{
		return;
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}
void ABlasterCharacter::ReceiveDamage(AActor* DamageActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return;
	}
	float DamageToHealth = DamageAmount;
	if (Shield > 0.f)
	{
		if (Shield >= DamageAmount)
		{
			Shield = FMath::Clamp(Shield - DamageAmount, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, DamageAmount);
			Shield = 0.f;
		}
	}
	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);
	UpdateHUDHealth();
	UpdateHUDShield();
	PlayHitReactMontage();

	if (Health == 0.f)
	{
		//Die
		ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
		if (BlasterGameMode)
		{
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatedBy);
			BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, AttackerController);

		}

	}

	
}
void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABlasterCharacter, Health, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABlasterCharacter, Shield, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABlasterCharacter, Killer, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABlasterCharacter, bDisableGameplayInput, COND_OwnerOnly);
}

void ABlasterCharacter::Move(const FInputActionValue& Value)
{
	if (bDisableGameplayInput)
	{
		return;
	}
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, ControlRotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);

}
void ABlasterCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();


	AddControllerPitchInput(LookVector.Y);
	AddControllerYawInput(LookVector.X);

}
void ABlasterCharacter::Equip(const FInputActionValue& Value)
{
	if (bDisableGameplayInput)
	{
		return;
	}
	if(CombatComponent && OverlappingWeapon)
	{
		ServerEquipButtonPressed();
	}
	
}
void ABlasterCharacter::SwitchWeapon(const FInputActionValue& Value)
{
	if (bDisableGameplayInput)
	{
		return;
	}
	if (CombatComponent)
	{
		ServerSwitchWeaponButtonPressed();
	}
}
void ABlasterCharacter::CrouchPressed(const FInputActionValue& Value)
{
	if (bDisableGameplayInput)
	{
		return;
	}
	if (Value.Get<float>() > 0.0f)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}
void ABlasterCharacter::AimPressed(const FInputActionValue& Value)
{
	if (bDisableGameplayInput)
	{
		return;
	}
	if (CombatComponent && IsWeaponEquipped())
	{
		CombatComponent->SetAiming(Value.Get<float>() > 0.0f);
	}
}
void ABlasterCharacter::ReloadPressed(const FInputActionValue& Value)
{
	if (bDisableGameplayInput)
	{
		return;
	}
	if (CombatComponent && IsWeaponEquipped())
	{
		CombatComponent->Reload();
	}
}
void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (CombatComponent && CombatComponent->EquippedWeapon == nullptr)
	{
		return;
	}
	const float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir)
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetControlRotation().Yaw, 0.f);
		FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation );
		Ao_Yaw = DeltaRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = Ao_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);

	}
	if (Speed > 0.f || bIsInAir)
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetControlRotation().Yaw, 0.f);
		Ao_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	}
	CalculateAO_Pitch();

}
void ABlasterCharacter::CalculateAO_Pitch()
{
	Ao_Pitch = GetBaseAimRotation().Pitch;
	if (Ao_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch to the range [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		Ao_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, Ao_Pitch);

	}
}
void ABlasterCharacter::SimProxiesTurn()
{

	if (CombatComponent && CombatComponent->EquippedWeapon == nullptr)
	{
		return;
	}
	float Speed = CalculateSpeed();
	bRotateRootBone = false;
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYawDelta = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;



	if (FMath::Abs(ProxyYawDelta) > TurnThreshold)
	{
		if (ProxyYawDelta > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYawDelta < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;


}
void ABlasterCharacter::FirePressed(const FInputActionValue& Value)
{
	if (bDisableGameplayInput)
	{
		return;
	}
	if (CombatComponent && IsWeaponEquipped())
	{
		if (Value.Get<float>() > 0.0f)
		{
			CombatComponent->FireButtonPressed(true);
		}
		else
		{
			CombatComponent->FireButtonPressed(false);
		}
	}


}

void ABlasterCharacter::ThrowGrenadePressed(const FInputActionValue& Value)
{
	if (bDisableGameplayInput)
	{
		return;
	}
	if (CombatComponent && IsWeaponEquipped())
	{
		CombatComponent->ThrowGrenade();
	}

}



void ABlasterCharacter::ServerSwitchWeaponButtonPressed_Implementation()
{
	if (CombatComponent->ShouldSwapWeapon())
	{
		CombatComponent->SwapWeapon();
	}
}


void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (Ao_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (Ao_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0, DeltaTime, 4.f);
		Ao_Yaw = InterpAO_Yaw;
		if (FMath::Abs(Ao_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetControlRotation().Yaw, 0.f);

		}

	}

}

void ABlasterCharacter::Jump()
{
	if (bDisableGameplayInput)
	{
		return;
	}
	Super::Jump();
}

