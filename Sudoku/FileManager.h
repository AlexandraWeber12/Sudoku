#pragma once

std::vector<std::string> getFileNames(std::string directoryPath)
{
	namespace fs = std::filesystem;
	std::vector<std::string> names;

	if (fs::exists(directoryPath)) {
		fs::directory_iterator it(directoryPath);
		fs::directory_iterator end;

		while (it != end) {
			names.push_back(it->path().filename().string());
			++it;
		}
	}
	return names;
}

void filter(std::vector<std::string>& strings, std::string pattern)
{
	auto pos = std::remove_if(std::begin(strings), std::end(strings),
		[&](std::string& s) { return s.find(pattern) == std::string::npos; });

	strings.erase(pos, std::end(strings));
}

void print(const std::vector<std::string>& strings)
{
	for (auto& s : strings)
		std::cout << s << '\n';
}

void printSolutionToFile(int& dim, SudokuGrid sg) {
	std::ofstream myFile;
	std::string filename = "Solution" + std::to_string(dim) + ".txt";
	myFile.open(filename);

	sg.printSudoku(myFile);
}