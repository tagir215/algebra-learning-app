// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FormulaTree.h"
#include "AlgebraSolver.h"
#include "PlayerPawn.generated.h"


UCLASS()
class ALGEBRASPP_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	FString ProcessKey(FText KeyName);


	UFUNCTION(BlueprintCallable)
	void CommitExpression(AlgebraicExpression operation, TArray<int> indexes);

	UFUNCTION(BlueprintCallable)
	FString GetExpressionAsString();

private:
	int currentIndex = 0;

	bool editingRight;
	bool solveMode;
	FormulaTree formulaTreeLeft;
	FormulaTree formulaTreeRight;

	FString ProcessInputMode(FString Key);
	FString ProcessSolveMode(FString Key);

};
