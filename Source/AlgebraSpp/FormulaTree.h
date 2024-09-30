// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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

class Value {
private:
	bool isValue;
	FString string;
public:
	void SetValue(FString value) {
		this->string = value;
		this->isValue = true;
	}

	FString GetValue() {
		return string;
	}

	bool HasValue() const {
		return isValue;
	}
};

struct FormulaUnit : TSharedFromThis<FormulaUnit> {
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

/**
 *
 */
class ALGEBRASPP_API FormulaTree
{
public:
	FormulaTree();
	~FormulaTree();

	unsigned int size = 0;

	void addUnit(TSharedPtr<FormulaUnit> unit) {
		++size;
		formulaArray.Push(unit);
		buildTree();
	}

	void buildTree();

	TSharedPtr<FormulaUnit>& GetUnitByIndex(unsigned int index) {
		return formulaArray[index];
	}

	TArray<TSharedPtr<FormulaUnit>> combineValues(const TArray<TSharedPtr<FormulaUnit>>& formula);

	TSharedPtr<FormulaUnit> buildBracketHierarchy(const TArray<TSharedPtr<FormulaUnit>>& formula);

	static void containerize(TSharedPtr<FormulaUnit>& container, UnitType type);

	void sortImplicitMultiplications(TSharedPtr<FormulaUnit>& container);

	void sortOperatorType(TSharedPtr<FormulaUnit>& container, UnitType type);

	TSharedPtr<FormulaUnit>& GetRoot() {
		return formulaRoot;
	}

	void deleteUnit(int index) {

	}

	void print() {
		FString string = "";
		stringify(formulaRoot, string);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *string);
	}

	FString getAsString() {
		FString string = "";
		stringify(formulaRoot, string);
		return string;
	}

	void stringify(TSharedPtr<FormulaUnit>& unit, FString& string) {

		if (unit->type == UnitType::Container) string += "(";


		for (int i = 0; i < unit->children.Num(); ++i) {
			auto& child = unit->children[i];
			stringify(child, string);
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
			string += unit->value.GetValue();
		}
		else if (unit->type == UnitType::Container) string += ")";
	}

private:
	TSharedPtr<FormulaUnit> formulaRoot;
	TArray<TSharedPtr<FormulaUnit>> formulaArray;

};
