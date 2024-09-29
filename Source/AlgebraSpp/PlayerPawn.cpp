// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

}


FString APlayerPawn::ProcessKey(FText KeyName)
{
	FString Key = KeyName.ToString();

	//UE_LOG(LogTemp, Warning, TEXT("Key pressed: %s"), *Key);

	TSharedPtr<FormulaUnit> unit = nullptr;

	if (Key.Equals("Right")) {
		Key = "+";
		unit = MakeShared<FormulaUnit>(UnitType::Addition);
		unit->isOperator = true;
	}
	else if (Key.Equals("Left")) {
		Key = "-";
		unit = MakeShared<FormulaUnit>(UnitType::Subtraction);
		unit->isOperator = true;
	}
	else if (Key.Equals("Up")) {
		Key = "*";
		unit = MakeShared<FormulaUnit>(UnitType::Multiplication);
		unit->isOperator = true;
	}
	else if (Key.Equals("Down")) {
		Key = "/";
		unit = MakeShared<FormulaUnit>(UnitType::Division);
		unit->isOperator = true;
	}
	else if (Key.Equals("Left Ctrl")) {
		Key = "^";
		unit = MakeShared<FormulaUnit>(UnitType::Exponentiation);
		unit->isOperator = true;
	}
	else if (Key.Equals("Left Shift")) {
		Key = "(";
		unit = MakeShared<FormulaUnit>(UnitType::OpeningBracket);
	}
	else if (Key.Equals("Right Shift")) {
		Key = ")";
		unit = MakeShared<FormulaUnit>(UnitType::ClosingBracket);
	}
	else if (Key.Equals("Space Bar")) {
		Key = "=";
		unit->isOperator = true;
	}

	if (Key.Len() > 1) return "";


	if (!unit) {
		if (Key.IsNumeric()) 
			unit = MakeShared<FormulaUnit>(UnitType::Value);
		else 
			unit = MakeShared<FormulaUnit>(UnitType::Variable);
		unit->value.string = Key;
	}

	unit->index = currentIndex++;
	formulaTree.addUnit(unit);
	formulaTree.print();

	return Key;
}

