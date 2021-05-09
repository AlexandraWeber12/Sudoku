
#include <vector>
#include <iostream>
#include <cmath>
#include <filesystem>
#include <string>
#include <fstream>
#include <vld.h>

#include "SudokuGrid.h"
#include "FileManager.h"
#include "TimeManager.h"

//a few of the SudokuExamples are renamed so that the algorith not solve them all every time because a few of them they need a lot of time
int main()
{
	auto files = getFileNames(std::filesystem::current_path().string());
	filter(files, "SudokuExample");
	std::vector<int> sudokuContent{};

	for (auto & file : files) {
		std::ifstream myFile(file);

		int dim{};
		int number{};
		if (myFile.is_open())
		{
			myFile >> dim;
			while (myFile >> number) {
				sudokuContent.push_back(number);
			}

			// solve sudoku
			SudokuGrid sudoku(dim);
			sudoku.prepareGrid();
			sudoku.fillGrid(sudokuContent);
			sudoku.prepareData();
			sudoku.buildCoordinateArray();
			sudoku.printSudoku(std::cout);

			auto const elapsed{ timed_run([&sudoku] {
			  sudoku.solveSudoku();
		   }) };

			sudoku.printSudoku(std::cout);
			printSolutionToFile(dim, sudoku);
			sudoku.releaseMemoryFromArrays();

			sudokuContent.clear();

			std::chrono::duration <double, std::ratio < 1>> const seconds{ elapsed };
			std::chrono::duration <double, std::ratio <60>> const minutes{ elapsed };

			std::cout
				<< "Time needed for "
				<< dim << "x" << dim 
				<< ": "
				<< elapsed << " == "
				<< seconds << " == "
				<< minutes << "\n"
						   << "\n";

			myFile.close();
		}
		else {
			std::cout << "Unable to open file";
		}
	}
}

