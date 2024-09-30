// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FormulaTree.h"
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class AlgebraicExpression : uint8 {
	BothSideAddition UMETA(DisplayName = "Both Side Addition"),
	BothSideSubtraction UMETA(DisplayName = "Both Side Subtraction"),
	BothSideMultipication UMETA(DisplayName = "Both Side Multipication"),
	BothSideDivision UMETA(DisplayName = "Both Side Division"),
	CombineTerms UMETA(DisplayName = "Combine Terms"),
	Factor UMETA(DisplayName = "Factor"),
	Distribute UMETA(DisplayName = "Distribute"),
	DivideCommonFactors UMETA(DisplayName = "Divide Common Factors"),
};


/**
 * 
 */
class ALGEBRASPP_API AlgebraSolver
{
public:

	AlgebraSolver(FormulaTree& leftTree, FormulaTree& rightTree) : leftTree(leftTree), rightTree(rightTree) {};
	~AlgebraSolver() {};

	void BothSideAddition(TSharedPtr<FormulaUnit>& unit);

	void CombineTerms(TSharedPtr<FormulaUnit>& unit1, TSharedPtr<FormulaUnit>& unit2);

	float GetValue(TSharedPtr<FormulaUnit>& unit);

	float StringToValue(FString string);

	template<typename Callable>
	float LoopChildrenWithOperator(TSharedPtr<FormulaUnit>& unit, Callable callable) {
		float total = GetValue(unit->children[0]);
		for (int i = 1; i < unit->children.Num(); ++i) {
			total = callable(total, unit->children[i]);
		}
		return total;
	}


private:
	FormulaTree& leftTree;
	FormulaTree& rightTree;
};
