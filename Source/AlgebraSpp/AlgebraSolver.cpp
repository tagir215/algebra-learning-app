// Fill out your copyright notice in the Description page of Project Settings.


#include "AlgebraSolver.h"
#include "Algo/ForEach.h"
#include <cmath>


void AlgebraSolver::BothSideAddition(TSharedPtr<FormulaUnit>& unit)
{
	TSharedPtr<FormulaUnit> copy1 = MakeShareable(new FormulaUnit(*unit.Get()));
	TSharedPtr<FormulaUnit> copy2 = MakeShareable(new FormulaUnit(*unit.Get()));
	auto& last1 = leftTree.GetRoot()->children.Last();
	auto& last2 = rightTree.GetRoot()->children.Last();

	if (last1->type != UnitType::Addition) {
		FormulaTree::containerize(last1, UnitType::Addition);
	}
	last1->addChild(copy1);

	if (last2->type != UnitType::Addition) {
		FormulaTree::containerize(last2, UnitType::Addition);
	}
	last2->addChild(copy2);
}

void AlgebraSolver::CombineTerms(TSharedPtr<FormulaUnit>& unit1, TSharedPtr<FormulaUnit>& unit2)
{
	
}

float AlgebraSolver::GetValue(TSharedPtr<FormulaUnit>& unit)
{
	if (unit->value.HasValue()) {
		return StringToValue(unit->value.GetValue());
	}

	if (unit->type == UnitType::Addition) {
		return LoopChildrenWithOperator(unit, [&](float total, TSharedPtr<FormulaUnit>& unit) { return total + GetValue(unit); });
	}
	if (unit->type == UnitType::Subtraction) {
		return LoopChildrenWithOperator(unit, [&](float total, TSharedPtr<FormulaUnit>& unit) { return total - GetValue(unit); });
	}
	if (unit->type == UnitType::Multiplication) {
		return LoopChildrenWithOperator(unit, [&](float total, TSharedPtr<FormulaUnit>& unit) { return total * GetValue(unit); });
	}
	if (unit->type == UnitType::Division) {
		return LoopChildrenWithOperator(unit, [&](float total, TSharedPtr<FormulaUnit>& unit) { return total / GetValue(unit); });
	}
	if (unit->type == UnitType::Exponentiation) {
		return LoopChildrenWithOperator(unit, [&](float total, TSharedPtr<FormulaUnit>& unit) { return std::pow(total, GetValue(unit)); });
	}

	return 0;
}

float AlgebraSolver::StringToValue(FString string)
{
	if (string.IsNumeric()) {
		return FCString::Atof(*string);
	}

	check(string.Len() <= 1 && "Error error String too long");

	return static_cast<float>(string[0]);
}

