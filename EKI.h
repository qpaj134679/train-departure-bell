#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <Windows.h>
#include <sstream>
#include <SFML/Audio.hpp>
#include <clocale>
#include <codecvt>

struct eki_sound_pair_hash
{
    template<class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

class EKI final
{
private:
    std::unordered_map<std::wstring, std::vector<std::wstring>> line_eki;  // save line-eki pairs
    std::unordered_map<std::pair<std::wstring, std::wstring>, std::wstring, eki_sound_pair_hash> eki_sound;  // save eki-sound pairs

public:
    EKI(const std::wstring data_path);
    EKI(const EKI&);
    ~EKI();
    std::vector<std::wstring> get_line_eki(const std::wstring);  // get all the stations of a line
    std::vector<std::wstring> get_lines();  // get all lines
    std::wstring GetSound(const std::wstring, const std::wstring);  // get sound path by specified line and station

};

