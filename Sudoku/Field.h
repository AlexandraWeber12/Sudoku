#pragma once

struct coordinates {
	int x{};
	int y{};
};

class Field
{
private:
	int value{};
	int dimension{};
	coordinates co{};

public:
	std::vector<int> possibleValues{};
	//the tryValue is used to not have to search the fixed values again and can be reset if the solution vector is wrong
	int tryValue{}; 

	Field() {}

	int getValue() {
		return value;
	}

	coordinates getCoord() {
		return co;
	}

	void setValue(int val) {
		value = val;
	}

	void setCooridnates(std::tuple<int, int> c) {
		co.x = std::get<0>(c);
		co.y = std::get<1>(c);
	}

	void reducePossibleValues(int val) {
		possibleValues.erase(std::remove_if(possibleValues.begin(), possibleValues.end(), [&](const auto& entry)
			{
				return entry == val;
			}), possibleValues.end());
	}
};

