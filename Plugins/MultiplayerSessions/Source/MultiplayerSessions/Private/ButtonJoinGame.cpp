// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonJoinGame.h"
#include <MultiplayerSessionsSubsystem.h>

void UButtonJoinGame::JoinGame()
{
	MultiplayerSubsystem->JoinSession(SearchResult);
}
