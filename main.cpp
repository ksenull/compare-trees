#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <iomanip>
#include <fstream>
#include <algorithm>

#include "avltree.h"
#include "hash.h"

using namespace std;
using namespace tree;
using namespace Hash;

#define TOSEC(a, b) ((double)((b)- (a)) / (double) CLOCKS_PER_SEC)

static const int MIN = 7;
static const int MAX = 20;
static const int HASH_TABLE_SIZE = 1000000;
static const int BLOCK_SIZE = 2000;
static const int FROM = 1000;
static const int TO = 11000;
static const int STEP = 5000;

int randInt(int min, int max) {
    return rand() % (max - min + 1) + min;
}

string randStr(const int len) {

    char *s = new char[len];
    static const char alphanum[] =
                    "0123456789"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz"
                    "абвгдеёжзийклмнопрстуфхцчшщъыьэюя"
                    "АБМГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
    return string(s);
}

struct SData {
    string key1;
    string key2;

    SData() : key1(randStr(randInt(MIN, MAX))), key2(randStr(randInt(MIN, MAX))) {}

    static int cmp(const SData *a, const SData *b) {
        int result = a->key1.compare(b->key1);
        if (result == 0) {
            return a->key2.compare(b->key2);
        } else {
            return result;
        }
    }

    static bool compare(const SData &a, const SData &b) {
        return cmp(&a, &b) < 0;
    }
};

unsigned int hashFunction(const SData *str) {
    return std::hash<string>()(str->key1) + std::hash<string>()(str->key2);
}

map<string, array<double, 3>> countTimes(int n) {
//    array<array<double, 5>, 3> times;
    map<string, array<double, 3>> times; // keys == actions, values: avl, hash, binsearch

    vector<SData> dataVector;
    vector<SData> dataVectorCopy;
    vector<SData> dataVectorForSearch;

    for (int i = 0; i < n; ++i) {
        dataVector.emplace_back(SData());
    }

    dataVectorCopy = dataVector;

    for (int l = 0; l < 2 * n; ++l) {
        dataVectorForSearch.emplace_back(SData());
    }

    CAVLTree<SData, SData::cmp> tree;
    CHash<SData, hashFunction, SData::cmp> table(HASH_TABLE_SIZE, BLOCK_SIZE);

    clock_t start, end;

    // Initialization
    start = clock();
    for (int i = 0; i < dataVector.size(); ++i) {
        tree.add(&dataVector[i]);
    }
    end = clock();
    times["init"][0] = TOSEC(start, end);

    start = clock();
    for (int i = 0; i < dataVector.size(); ++i) {
        table.add(&dataVector[i]);
    }
    end = clock();
    times["init"][1] = TOSEC(start, end);

    start = clock();
    sort(dataVectorCopy.begin(), dataVectorCopy.end(), SData::compare);
    end = clock();
    times["init"][2] = TOSEC(start, end);

    // Search all obj included
    start = clock();
    for (int i = 0; i < dataVector.size(); ++i) {
        tree.find(dataVector[i]);
    }
    end = clock();
    times["search_all"][0] = TOSEC(start, end);

    start = clock();
    for (int i = 0; i < dataVector.size(); ++i) {
        table.find(dataVector[i]);
    }
    end = clock();
    times["search_all"][1] = TOSEC(start, end);

    start = clock();
    for (int i = 0; i < dataVector.size(); ++i) {
        lower_bound(dataVectorCopy.begin(), dataVectorCopy.end(), dataVector[i], SData::compare);
    }
    end = clock();
    times["search_all"][2] = TOSEC(start, end);


    // Search random
    start = clock();
    for (int i = 0; i < dataVectorForSearch.size(); ++i) {
        tree.find(dataVectorForSearch[i]);
    }
    end = clock();
    times["search_random"][0] = TOSEC(start, end);

    start = clock();
    for (int i = 0; i < dataVectorForSearch.size(); ++i) {
        table.find(dataVectorForSearch[i]);
    }
    end = clock();
    times["search_random"][1] = TOSEC(start, end);

    start = clock();
    for (int i = 0; i < dataVectorForSearch.size(); ++i) {
        lower_bound(dataVectorCopy.begin(), dataVectorCopy.end(), dataVectorForSearch[i], SData::compare);
    }
    end = clock();
    times["search_random"][2] = TOSEC(start, end);

    // Remove every elem
    start = clock();
    for (int i = 0; i < dataVector.size(); ++i) {
        tree.remove(dataVector[i]);
    }
    end = clock();
    times["remove"][0] = TOSEC(start, end);

    start = clock();
    for (int i = 0; i < dataVector.size(); ++i) {
        table.remove(dataVector[i]);
    }
    end = clock();
    times["remove"][1] = TOSEC(start, end);

    times["remove"][2] = 0; // measure


    // Free
    for (int i = 0; i < dataVector.size(); ++i) {
        tree.add(&dataVector[i]);
        table.add(&dataVector[i]);
    }

    start = clock();
    tree.clear();
    end = clock();
    times["free"][0] = TOSEC(start, end)

    start = clock();
    table.clear();
    end = clock();
    times["free"][1] = TOSEC(start, end)

    start = clock();
    dataVector.clear();
    end = clock();
    times["free"][2] = TOSEC(start, end);
    return times;
};

void printTable(map<string, array<double, 3>> times, int n, ofstream &file) {
    file << n <<
    ", Время сортировки/внесения данных, Время поиска №1, Время поиска №2, Время освобождения, Удаление по одному" <<
    std::endl;

    for (int i = 0; i < 3; ++i) {
        if (i == 0) {
            file << "AvlTree, ";
        } else if (i == 1) {
            file << "HashTable, ";
        } else if (i == 2) {
            file << "SortedArray, ";
        }
        for (int j = 0; j < 5; ++j) {
            file << times[i][j] << ", ";
        }
        file << endl;
    }
    file << endl;
}

void writeToCsv(string& header, string& columns, auto values) {

}

int main() {

    setlocale(LC_CTYPE, "rus");
    srand(time(0));

    ofstream file;
    cout << "Opening file for writing" << endl;
    file.open("table.csv");
    vector<map<string, array<double, 3>>> timesTables;
    for (int i = FROM; i <= TO; i += STEP) {
        map<string, array<double, 3>> tmpTable = countTimes(i);
        cout << "Printed table " << i << endl;
        printTable(tmpTable, i, file);
        timesTables.push_back(tmpTable);
    }
    file.close();

    file.open("graphs.csv");
    file << "Сортировка/внесение данных" << std::endl;
    file << ", ";
    for (int i = FROM; i < TO; i += STEP) {
        file << i << ", ";
    }
    file << std::endl;
    file << "AvlTree, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][0][0] << ", ";
    }
    file << std::endl;
    file << "HashTable, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][1][0] << ", ";
    }
    file << std::endl;
    file << "Array, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][2][0] << ", ";
    }

    file << std::endl;
    file << std::endl;
    file << "Поиск №1" << std::endl;
    file << ", ";
    for (int i = FROM; i < TO; i += STEP) {
        file << i << ", ";
    }
    file << std::endl;
    file << "AvlTree, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][0][1] << ", ";
    }
    file << std::endl;
    file << "HashTable, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][1][1] << ", ";
    }
    file << std::endl;
    file << "Array, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][2][1] << ", ";
    }

    file << std::endl;
    file << std::endl;
    file << "Поиск №2" << std::endl;
    file << ", ";
    for (int i = FROM; i < TO; i += STEP) {
        file << i << ", ";
    }
    file << std::endl;
    file << "AvlTree, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][0][2] << ", ";
    }
    file << std::endl;
    file << "HashTable, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][1][2] << ", ";
    }
    file << std::endl;
    file << "Array, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][2][2] << ", ";
    }

    file << std::endl;
    file << std::endl;
    file << "Освобождение всего" << std::endl;
    file << ", ";
    for (int i = FROM; i < TO; i += STEP) {
        file << i << ", ";
    }
    file << std::endl;
    file << "AvlTree, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][0][3] << ", ";
    }
    file << std::endl;
    file << "HashTable, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][1][3] << ", ";
    }
    file << std::endl;
    file << "Array, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][2][3] << ", ";
    }

    file << std::endl;
    file << std::endl;
    file << "Удаление по одному" << std::endl;
    file << ", ";
    for (int i = FROM; i < TO; i += STEP) {
        file << i << ", ";
    }
    file << std::endl;
    file << "AvlTree, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][0][4] << ", ";
    }
    file << std::endl;
    file << "HashTable, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][1][4] << ", ";
    }
    file << std::endl;
    file << "Array, ";
    for (int i = 0; i < timesTables.size(); ++i) {
        file << timesTables[i][2][4] << ", ";
    }

    file.close();

    return 0;
}