#pragma once
#define LOCTEXT_NAMESPACE "Announcement"
namespace Announcement
{
	//const FString NewMatchStartsIn(TEXT("New Match Starts In : "));
	const FString NewMatchStartsIn = NSLOCTEXT("Announcement", "NewMatchStartsIn", "New Match Starts In : ").ToString();
	//const FString ThereIsNoWinner(TEXT("There is no winner"));
	//const FString YouAreTheWinner(TEXT("You are the winner!"));
	//const FString ThereIsATieBetween(TEXT("There is a tie between: "));
	//const FString ThereIsATieBetweenTheTeams(TEXT("There is a tie between the teams!"));
	//const FString RedTeamIsTheWinner(TEXT("Red Team win!"));
	//const FString BlueTeamIsTheWinner(TEXT("Blue Team win!"));
	const FString ThereIsNoWinner = NSLOCTEXT("Announcement", "ThereIsNoWinner", "There is no winner").ToString();
	const FString YouAreTheWinner = NSLOCTEXT("Announcement", "YouAreTheWinner", "You are the winner!").ToString();
	const FString ThereIsATieBetween = NSLOCTEXT("Announcement", "ThereIsATieBetween", "There is a tie between: ").ToString();
	const FString ThereIsATieBetweenTheTeams = NSLOCTEXT("Announcement", "ThereIsATieBetweenTheTeams", "There is a tie between the teams!").ToString();
	const FString RedTeamIsTheWinner = NSLOCTEXT("Announcement", "RedTeamIsTheWinner", "Red Team win!").ToString();
	const FString BlueTeamIsTheWinner = NSLOCTEXT("Announcement", "BlueTeamIsTheWinner", "Blue Team win!").ToString();

}
#undef LOCTEXT_NAMESPACE