#pragma once

#include "Field.h"

class SudokuGrid
{
private:
    std::vector<int> Solution{};
    int dim{};
    int numberOfFields{};
    Field** DataArray{};
    coordinates** SectionArray{};
    coordinates* CoordinateArray{};
    int cntCoordinatesToSolve{};
    bool solutionFound{ false };

    bool verifyValue(int & cntFields, int & element) {
        return  checkRow(CoordinateArray[cntFields].x, element) &&
                checkCol(CoordinateArray[cntFields].y, element) &&
                checkSection(CoordinateArray[cntFields].x, CoordinateArray[cntFields].y, element);
    }

    bool checkRow(int & col, int & val) {
        for (int row = 0; row < dim; row++) {
            if (DataArray[col][row].tryValue == val) {
                return false;
            }
        }
        return true;
    }

    bool checkCol(int & row, int & val) {
        for (int col = 0; col < dim; col++) {
            if (DataArray[col][row].tryValue == val) {
                return false;
            }
        }
        return true;
    }

    bool checkSection(int & row, int & col, int & val) {
        int sqrtDim = static_cast<int> (sqrt(dim));
        int x = row - (row % sqrtDim);
        int y = col - (col % sqrtDim);
        for (int section = 0; section < dim; section++) {
            if (SectionArray[section][0].x == x && SectionArray[section][0].y == y) {
                for (int secElements = 0; secElements < dim; secElements++) {
                    if (DataArray[SectionArray[section][secElements].x][SectionArray[section][secElements].y].tryValue == val) {
                        return false;
                    }
                }
                break;
            }
        }
        return true;
    }

    void prepareDataBySection(int& i, int& j) {
        int sqrtDim = static_cast<int> (sqrt(dim));
        //get sector out of coordinate
        int x = i - (i % sqrtDim);
        int y = j - (j % sqrtDim);
        for (int section = 0; section < dim; section++) {
            if (SectionArray[section][0].x == x && SectionArray[section][0].y == y) { //find matching column
                for (int secElements = 0; secElements < dim; secElements++) {
                    if (DataArray[SectionArray[section][secElements].x][SectionArray[section][secElements].y].getValue() != 0) {
                        DataArray[i][j].reducePossibleValues(DataArray[SectionArray[section][secElements].x][SectionArray[section][secElements].y].getValue());
                    }
                }
                //break because the column was found and the row was searched and reduced
                break;
            }
        }
    }

    void prepareDataByColumn(int& i, int& j) {
        for (int spalte = 0; spalte < dim; spalte++) {
            if (DataArray[spalte][j].getValue() != 0) {
                DataArray[i][j].reducePossibleValues(DataArray[spalte][j].getValue());
            }
        }
    }

    void prepareDataByRow(int& i, int& j) {
        for (int zeile = 0; zeile < dim; zeile++) {
            if (DataArray[i][zeile].getValue() != 0) {
                DataArray[i][j].reducePossibleValues(DataArray[i][zeile].getValue());
            }
        }
    }

    void createSectors() {
        int cntSector{ 0 };
        //only possible for sudkos with square sections -> get every corner of each section
        //section = sqrt(dim) x sqrt(dim) , there are dim * sections in each sudoku
        for (int x = 0; x < dim; x += static_cast<int> (sqrt(dim))) {
            for (int y = 0; y < dim; y += static_cast<int> (sqrt(dim))) {
                SectionArray[cntSector][0].x = x;
                SectionArray[cntSector][0].y = y;
                cntSector++;
            }
        }
    }

    void addCoordinatesToSector() {
        //add all coordinates from one section in one row to easy access them later
        int cntColumn{ 0 };
        for (int sec = 0; sec < dim; sec++) {
            for (int i = SectionArray[sec][0].x; i < (SectionArray[sec][0].x + sqrt(dim)); i++) {
                for (int j = SectionArray[sec][0].y; j < (SectionArray[sec][0].y + sqrt(dim)); j++) {
                    SectionArray[sec][cntColumn].x = i;
                    SectionArray[sec][cntColumn].y = j;
                    cntColumn++;
                }
            }
            cntColumn = 0;
        }
    }

public:
	SudokuGrid(int dim) : dim(dim), numberOfFields(dim*dim) {}

	void prepareGrid() {
        SectionArray = new coordinates * [dim];
        for (int i = 0; i < dim; ++i)
            SectionArray[i] = new coordinates[dim];

        createSectors();
        addCoordinatesToSector();
	}

    void buildCoordinateArray() {
        //numberOfFields is too big but the real size is only known afterwards (=cntCoordinatesToSolve)
        //the CoordinateArray ist an array of coordinates where the recursion could easy get the next needed coordinate
        CoordinateArray = new coordinates[numberOfFields];
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                if (DataArray[i][j].getValue() == 0) {
                    CoordinateArray[cntCoordinatesToSolve].x = i;
                    CoordinateArray[cntCoordinatesToSolve].y = j;
                    cntCoordinatesToSolve++;
                }
            }
        }
    }

    void fillGrid(std::vector<int> sudokuContent) {
        //the DataArray is filled with the data from a file
        DataArray = new Field * [dim];
        for (int i = 0; i < dim; ++i)
            DataArray[i] = new Field[dim];
        
        int zeilenIdx = 0;
        for (int i = 0; i < numberOfFields; i++) {
            DataArray[zeilenIdx][i % dim].setValue(sudokuContent[i]);
            DataArray[zeilenIdx][i % dim].setCooridnates({ zeilenIdx , i % dim });
            if (sudokuContent[i] == 0) {
                //Add every possible value to easy field and delete the known later
                for (int j = 1; j <= dim; j++) {
                    DataArray[zeilenIdx][i % dim].possibleValues.push_back(j);
                }
            }
            if (((i + 1) % dim) == 0) {
                zeilenIdx++;
            }
        }
    }

    //This function reduces the search space by...
    void prepareData() {
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                if (DataArray[i][j].getValue() == 0) {
                    prepareDataByRow(i, j);
                    prepareDataByColumn(i, j);
                    prepareDataBySection(i, j);
                }
            }
        }
    }

    void releaseMemoryFromArrays()
    {
        for (int i = 0; i < dim; ++i) {
            delete[] DataArray[i];
        }
        delete[] DataArray;

        for (int i = 0; i < dim; ++i) {
            delete[] SectionArray[i];
        }
        delete[] SectionArray;

        delete[] CoordinateArray;
    }

    //This function uses the back tracking algorithm
	void solveSudoku(int cntFields = 0) {
        for (auto element : DataArray[CoordinateArray[cntFields].x][CoordinateArray[cntFields].y].possibleValues) {
            if (verifyValue(cntFields, element)) {
                DataArray[CoordinateArray[cntFields].x][CoordinateArray[cntFields].y].tryValue = element;
                Solution.push_back(element);

                if (cntFields == (cntCoordinatesToSolve - 1)) {
                    for (int setIdx = 0; setIdx < cntCoordinatesToSolve; setIdx++) {
                        DataArray[CoordinateArray[setIdx].x][CoordinateArray[setIdx].y].setValue(Solution[setIdx]);
                    }
                    solutionFound = true;
                    return;
                }

                solveSudoku(cntFields + 1);

                if (solutionFound) {
                    return;
                }
                else {
                    Solution.pop_back();
                    DataArray[CoordinateArray[cntFields].x][CoordinateArray[cntFields].y].tryValue = 0;
                }
            }
        }
	}

    void printSudoku(std::ostream& os) {
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                os << DataArray[i][j].getValue() << " ";
            }
            os << "\n";
        }
        os << "\n";
    }
};