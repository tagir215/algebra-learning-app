// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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

private:

	int currentIndex = 0;

	enum class UnitType {
		Container,
		Value,
		Variable,
		Addition,
		Subtraction,
		Multiplication,
		Division,
		Exponentiation,
		OpeningBracket,
		ClosingBracket
	};

	struct Value {
		FString string;
	};

	struct FormulaUnit : TSharedFromThis<FormulaUnit>{
		UnitType type;
		Value value;
		int index;
		TArray<TSharedPtr<FormulaUnit>> children;
		TWeakPtr<FormulaUnit> parent;
		bool isOperator;

		void addChild(TSharedPtr<FormulaUnit>& child) {
			child->parent = AsShared();
			children.Push(child);
		}

		FormulaUnit(UnitType type) : type(type), parent(nullptr), isOperator(false) {
			
		}

		~FormulaUnit() {
		}
	};

	class FormulaTree {
	public:

		void addUnit(TSharedPtr<FormulaUnit> unit) {
			formulaArray.Push(unit);
			buildTree();
		}

		void buildTree() {
			TArray<TSharedPtr<FormulaUnit>> processedFormula; 
			processedFormula = combineValues(formulaArray);
			TSharedPtr<FormulaUnit> root = buildBracketHierarchy(processedFormula);
			sortImplicitMultiplications(root);
			sortOperatorType(root, UnitType::Exponentiation);
			sortOperatorType(root, UnitType::Division);
			sortOperatorType(root, UnitType::Multiplication);
			sortOperatorType(root, UnitType::Subtraction);
			sortOperatorType(root, UnitType::Addition);

			formulaRoot = root;
			if (processedFormula.Num() > 10) {
				int a = 0;
			}
		}

		TArray<TSharedPtr<FormulaUnit>> combineValues(const TArray<TSharedPtr<FormulaUnit>>& formula) {
			TArray<TSharedPtr<FormulaUnit>> result;
			TSharedPtr<FormulaUnit> combinedUnit = nullptr;
			for (TSharedPtr<FormulaUnit> unit : formula) {
				if (unit->type != UnitType::Value) {
					combinedUnit = nullptr;
					result.Push(unit);
				}
				else if (combinedUnit) {
					combinedUnit->value.string += unit->value.string;
				}
				else {
					combinedUnit = MakeShared<FormulaUnit>(UnitType::Value);
					combinedUnit->value = unit->value;
					result.Push(combinedUnit);
				}
			}
			return result;
		}

		TSharedPtr<FormulaUnit> buildBracketHierarchy(const TArray<TSharedPtr<FormulaUnit>>& formula) {
			auto root = MakeShared<FormulaUnit>(UnitType::Container);
			TSharedPtr<FormulaUnit> container = root;

			for (TSharedPtr<FormulaUnit> unit : formula) {
				if (unit->type == UnitType::OpeningBracket) {
					TSharedPtr<FormulaUnit> bracketsContainer = MakeShared<FormulaUnit>(UnitType::Container);
					container->addChild(bracketsContainer);
					container = bracketsContainer;
				}
				else if (unit->type == UnitType::ClosingBracket) {
					container = container->parent.Pin();
				}
				else {
					container->addChild(unit);
				}
			}
			return root;
		}

		void containerize(TSharedPtr<FormulaUnit>& container,  UnitType type) {
			auto originalUnit = container;
			container = MakeShared<FormulaUnit>(type);
			container->addChild(originalUnit);
		}

		void sortImplicitMultiplications(TSharedPtr<FormulaUnit>& container) {
			TArray<TSharedPtr<FormulaUnit>> result;
			const int num = container->children.Num();
			for (auto& unit : container->children) {
				if (unit->type == UnitType::Container) {
					sortImplicitMultiplications(unit);
				}
				if (result.Num() == 0) {
					result.Push(unit);
					continue;
				}

				auto& last = result.Last();
				if (unit->type == UnitType::Variable || last->type == UnitType::Container || unit->type == UnitType::Container) {
					if (last->type == UnitType::Value || last->type == UnitType::Variable
						|| unit->type == UnitType::Container || last->type == UnitType::Container) {
						if (last->type != UnitType::Multiplication) {
							containerize(last, UnitType::Multiplication);
						}
						last->addChild(unit);
					}
					else {
						result.Push(unit);
					}
				}
				else {
					result.Push(unit);
				}
			}
			container->children = result;
		}

		void sortOperatorType(TSharedPtr<FormulaUnit>& container, UnitType type) {
			TArray<TSharedPtr<FormulaUnit>> result;
			bool lastIsType = false;
			for (auto& unit : container->children) {
				if (unit->children.Num() > 0) {
					sortOperatorType(unit, type);
				}
				if (result.Num() == 0) {
					result.Push(unit);
					continue;
				}

				auto& last = result.Last();
				if (unit->type == type && unit->isOperator) {
					if (last->type != type) {
						containerize(last, type);
					}
					lastIsType = true;
				}
				else if (lastIsType) {
					last->addChild(unit);
					lastIsType = false;
				}
				else {
					result.Push(unit);
				}

			}
			container->children = result;
		}


		void deleteUnit(int index) {

		}

		FormulaTree() {
		}

		~FormulaTree() {
		}

		void print() {
			FString string = "";
			print(formulaRoot, string);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *string);
		}

		void print(TSharedPtr<FormulaUnit>& unit, FString& string) {

			if (unit->type == UnitType::Container) string += "(";


			for (int i = 0; i < unit->children.Num(); ++i) {
				auto& child = unit->children[i];
				print(child, string);
				if (i < unit->children.Num() - 1) {
					if (unit->type == UnitType::Addition) string += "+";
					else if (unit->type == UnitType::Multiplication) string += "*";
					else if (unit->type == UnitType::Subtraction) string += "-";
					else if (unit->type == UnitType::Division) string += "/";
					else if (unit->type == UnitType::Exponentiation) string += "^";
					else string += "?";
				}
			}

			if (unit->type == UnitType::Value || unit->type == UnitType::Variable) {
				string += unit->value.string;
			}
			else if (unit->type == UnitType::Container) string += ")";
		}

	private:
		TSharedPtr<FormulaUnit> formulaRoot;
		TArray<TSharedPtr<FormulaUnit>> formulaArray;
	};

	bool editingRight;
	FormulaTree formulaTreeLeft;
	FormulaTree formulaTreeRight;
};
