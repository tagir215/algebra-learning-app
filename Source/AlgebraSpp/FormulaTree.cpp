// Fill out your copyright notice in the Description page of Project Settings.


#include "FormulaTree.h"

FormulaTree::FormulaTree()
{
}

FormulaTree::~FormulaTree()
{
}

void FormulaTree::buildTree() {
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

TArray<TSharedPtr<FormulaUnit>> FormulaTree::combineValues(const TArray<TSharedPtr<FormulaUnit>>& formula) {
	TArray<TSharedPtr<FormulaUnit>> result;
	TSharedPtr<FormulaUnit> combinedUnit = nullptr;
	for (TSharedPtr<FormulaUnit> unit : formula) {
		if (unit->type != UnitType::Value) {
			combinedUnit = nullptr;
			result.Push(unit);
		}
		else if (combinedUnit) {
			combinedUnit->value.GetValue() += unit->value.GetValue();
		}
		else {
			combinedUnit = MakeShared<FormulaUnit>(UnitType::Value);
			combinedUnit->value = unit->value;
			result.Push(combinedUnit);
		}
	}
	return result;
}

TSharedPtr<FormulaUnit> FormulaTree::buildBracketHierarchy(const TArray<TSharedPtr<FormulaUnit>>& formula) {
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

void FormulaTree::containerize(TSharedPtr<FormulaUnit>& container, UnitType type) {
	auto originalUnit = container;
	container = MakeShared<FormulaUnit>(type);
	container->addChild(originalUnit);
}

void FormulaTree::sortImplicitMultiplications(TSharedPtr<FormulaUnit>& container) {
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

void FormulaTree::sortOperatorType(TSharedPtr<FormulaUnit>& container, UnitType type) {
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
