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

	if (solveMode) return ProcessSolveMode(Key);
	else return ProcessInputMode(Key);
}



void APlayerPawn::CommitExpression(AlgebraicExpression operation, TArray<int> indexes)
{
	if (operation == AlgebraicExpression::BothSideAddition) {
		unsigned int index = indexes[0];
		FormulaTree tree = index < formulaTreeLeft.size ? formulaTreeLeft : formulaTreeRight;
		auto& unit = tree.GetUnitByIndex(index);
	}
}

FString APlayerPawn::GetExpressionAsString()
{
	return formulaTreeLeft.getAsString() + " = " + formulaTreeRight.getAsString();
}

FString APlayerPawn::ProcessInputMode(FString Key) {

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
		editingRight = true;
		return Key;
	}
	else if (Key.Equals("Enter")) {
		solveMode = true;
		return "";
	}

	if (Key.Len() > 1) return "";


	if (!unit) {
		if (Key.IsNumeric()) 
			unit = MakeShared<FormulaUnit>(UnitType::Value);
		else 
			unit = MakeShared<FormulaUnit>(UnitType::Variable);
		unit->value.SetValue(Key);
	}

	unit->index = currentIndex++;


	if (editingRight) {
		formulaTreeRight.addUnit(unit);
		formulaTreeRight.print();
	}
	else {
		formulaTreeLeft.addUnit(unit);
		formulaTreeLeft.print();
	}


	return Key;
}

FString APlayerPawn::ProcessSolveMode(FString Key)
{
	return FString();
}
