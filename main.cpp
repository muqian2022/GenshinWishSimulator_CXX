#include "lib/json.hpp"
#include "lib/random.hpp"

#include <conio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


using json = nlohmann::json;
using Random = effolkronium::random_static;
using namespace std;

json init_json(string path) {
    std::ifstream f(path);
    json data = json::parse(f);
    f.close();
    return data;
}

class Probability {
private:
    const int _5star_max = 90 + 1;
    const int _4star_max = 10 + 1;
    const double _5star_basic_probability = 0.006;
    // const double _4star_basic_probability = 0.051;

    // double _5star_curr_probability = 0.006;
    // double _4star_curr_probability = 0.05;

    std::vector<double> _5star_probability_list;
    std::vector<double> _4star_probability_list = {
            // 后面一堆1是为了防止某些人 欧到家了导致vector访问越界
            0.051, 0.051, 0.051, 0.051, 0.051, 0.051, 0.051, 0.056, 0.561, 1, 1, 1, 1, 1, 1, 1, 1};

public:
    Probability() {
        _5star_probability_list.resize(_5star_max, _5star_basic_probability);
        for (int i = 74; i < _5star_probability_list.size(); i++) {
            _5star_probability_list[i] = _5star_probability_list[i - 1] + 0.06;
        }
        _5star_probability_list[90] = 1;
    }
    bool is_5star(int x) {
        return Random::get<bool>(_5star_probability_list[x]);
    }
    bool is_4star(int x) {
        return Random::get<bool>(_4star_probability_list[x]);
    }

    void print() {
        for (auto item: _5star_probability_list) {
            cout << item << " ";
        }
    }

    void about() {
        cout << "\n\n// 概率参考：https://www.bilibili.com/read/cv12616453/\n作者：OneBST";
    }
};

class BannerItems {
private:
    vector<string> _5star_up;
    vector<string> _4star_up;
    vector<string> _5star_std;
    vector<string> _4star_std;
    vector<string> _3star_std;

public:
    BannerItems(json data) {
        _5star_up = data["5_star_up"];
        _4star_up = data["4_star_up"];
        _5star_std = data["5_star_standard"];
        _4star_std = data["4_star_standard"];
        _3star_std = data["3_star_standard"];
    }
    BannerItems() = default;
    // "\033[33mYellow Text\033[0m"
    // "\033[35mMagenta Text\033[0m"

    string get_a_5star_up() {
        return "\033[33m" + *Random::get(_5star_up) + "\033[0m";
    }
    string get_a_4star_up() {
        return "\033[35m" + *Random::get(_4star_up) + "\033[0m";
    }
    string get_a_5star_std() {
        return "\033[33m" + *Random::get(_5star_std) + "\033[0m";
    }
    string get_a_4star_std() {
        return "\033[35m" + *Random::get(_4star_std) + "\033[0m";
    }
    string get_a_3star_std() { return *Random::get(_3star_std); }


    void print() {
        for (const auto item: _5star_std) {
            cout << item << " ";
        }
    }
};

class SillyWisher {
private:
    bool is_5star_up{false}, is_4star_up{false};
    int to_5star{};
    int to_4star{};
    const int _5star_max = 90; // 没用到
    const int _4star_max = 10;

    BannerItems bi;
    Probability pr;

public:
    SillyWisher(BannerItems bi_) {
        bi = bi_;
    }

    string wish_once() {
        to_4star++, to_5star++;
        if (pr.is_5star(to_5star)) {
            to_5star = 0;
            if (is_5star_up) {
                is_5star_up = false;
                return bi.get_a_5star_up();
            }
            is_5star_up = Random::get<bool>(0.5);
            is_5star_up = !is_5star_up;
            return !is_5star_up ? bi.get_a_5star_up() : bi.get_a_5star_std();

        } else if (pr.is_4star(to_4star)) {
            to_4star = 0;
            if (is_4star_up) {
                is_4star_up = false;
                return bi.get_a_4star_up();
            }
            is_4star_up = Random::get<bool>(0.5);
            is_4star_up = !is_4star_up;
            return is_4star_up ? bi.get_a_4star_up() : bi.get_a_4star_std();
        } else {
            return bi.get_a_3star_std();
        }
    }
    void set(int a, int b) {
        to_4star = a;
        to_5star = b;
    }

    vector<string> wish_10_times() {
        vector<string> vec;
        for (int i = 0; i < 10; i++)
            vec.push_back(wish_once());
        return vec;
    }
};

int main() {
    // third-party library
    //
    // JSON for Modern C++
    // version 3.11.3
    // https://github.com/nlohmann/json
    // SPDX-FileCopyrightText: 2013-2023 Niels Lohmann <https://nlohmann.me>
    // SPDX-License-Identifier: MIT
    //
    // Random for modern C++
    // version 1.5.0
    // https://github.com/effolkronium/random
    // Licensed under the MIT License <http://opensource.org/licenses/MIT>.
    // Copyright (c) 2017-2023 effolkronium (Illia Polishchuk)
    //
    // 概率参考：https://www.bilibili.com/read/cv12616453/
    //
    
    json info = init_json("./Settings.json");
    BannerItems bi(info);
    SillyWisher sw(info);
    int a, b, times;// a、b 是距离上一个4星、5星之后进行了多少抽
    cin >> a >> b >> times;
    sw.set(a, b);
    while (times > 0) {
        char x = _getch();
        if (x == '1') {
            times--;
            cout << "Wish Once: " << sw.wish_once() << '\n';
        }
        if (x == '2') { // 按理来说，如果不满十抽 不可以进行的
            times -= 10;
            cout << "Wish 10 Times: ";
            for (auto item: sw.wish_10_times()) {
                cout << item << " ";
            }
            cout << '\n';
        }
    }
    std::cout << "\033[31;4m--END--\033[0m" << std::endl;
    cin.ignore();
}