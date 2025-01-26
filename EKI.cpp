#include "EKI.h"
#include "utils.h"

using namespace utils;

//
//  FUNCTION: EKI()
//
//  PURPOSE: Constructor, data_path: eki data path
//
EKI::EKI(const std::wstring data_path) {
	// read eki data from file
	HANDLE hFile = CreateFile(data_path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		throw RunTimeException("Failed to open file");
	}
	DWORD file_size = GetFileSize(hFile, NULL);
	DWORD bytesRead;
	std::wstring file_str = L"";
	char* data = new char[file_size];
	try {
		if (!ReadFile(hFile, data, file_size, &bytesRead, NULL)) {
			CloseHandle(hFile);
			throw RunTimeException("Failed to read file");
		}
		CloseHandle(hFile);
		int data_size = MultiByteToWideChar(CODE_PAGE, 0, data, -1, NULL, 0);
		wchar_t* w_data = new wchar_t[data_size + 1];
		if (!MultiByteToWideChar(CP_ACP, 0, data, -1, w_data, data_size + 1)) {
			delete[] w_data;
			throw RunTimeException("Failed to read file");
		}
		file_str = std::wstring(w_data);
		delete[] w_data;
	}
	catch (const std::exception&) {
		delete[] data;
		throw;
	}

	// analyze line-eki-sound pair
	size_t startPos = file_str.find(L"#$START$#");
	size_t endPos = file_str.find(L"#$END$#");
	if ((startPos == std::wstring::npos || endPos == std::wstring::npos) || (startPos >= endPos)) {
		throw RunTimeException("Invalid file");
	}

	std::wstring line_eki_str = file_str.substr(startPos + wcslen(L"#$START$#"), endPos - startPos - wcslen(L"#$END$#"));
	std::wstringstream ss_line_eki(line_eki_str);
	std::wstring line_str = L"";
	std::wstring eki_str = L"";
	std::wstring sound_path_str = L"";
	while (ss_line_eki >> line_str >> eki_str >> sound_path_str) {
		auto it1 = line_eki.find(line_str);
		if (it1 == line_eki.end()) {
			std::vector<std::wstring> line;
			line.push_back(eki_str);
			line_eki.insert({ line_str, line });
		}
		else {
			it1->second.push_back(eki_str);
		}
		eki_sound.insert({ std::pair<std::wstring, std::wstring>(line_str, eki_str), sound_path_str });
	}
}

EKI::EKI(const EKI& e) {
	line_eki = e.line_eki;
	eki_sound = e.eki_sound;
}

EKI::~EKI() {}

//
//  FUNCTION: EKI::get_lines()
//
//  PURPOSE: get all lines
//
std::vector<std::wstring> EKI::get_lines() {
	std::vector<std::wstring> result;
	for (auto iter = line_eki.begin(); iter != line_eki.end(); ++iter) {
		result.push_back(iter->first);
	}
	return result;
}

//
//  FUNCTION:EKI::get_line_eki(const std::wstring)
//
//  PURPOSE: get all the stations of a line
//
std::vector<std::wstring> EKI::get_line_eki(const std::wstring line) {
	auto it = line_eki.find(line);
	if (it == line_eki.end()) {
		return std::vector<std::wstring>();
	}
	else
	{
		return it->second;
	}
}

//
//  FUNCTION:EKI::GetSound(wstring, wstring)
//
//  PURPOSE: get sound path by specified line and station
//
std::wstring EKI::GetSound(const std::wstring sLine, const std::wstring sStation) {
	std::wstring result = L"";
	std::pair<std::wstring, std::wstring> pLine_station(sLine, sStation);
	auto it = eki_sound.find(pLine_station);
	if (it != eki_sound.end()) {
		result = it->second;
	}
	return result;
}