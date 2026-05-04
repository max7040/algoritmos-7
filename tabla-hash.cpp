//codigo en c++ del PDF - sin modiicacion
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <chrono>
#include <string>

using namespace std;

struct Record {
    string key;
    string value;
};

class HashTableChaining {
private:
    int size;
    vector<list<Record>> table;
    int collisions;

public:
    HashTableChaining(int tableSize) {
        size = tableSize;
        table.resize(size);
        collisions = 0;
    }

    int hashFunction(string key) {
        unsigned long hash = 0;

        for (char c : key) {
            hash = hash * 31 + c;
        }

        return hash % size;
    }

    void insert(string key, string value) {
        int index = hashFunction(key);

        if (!table[index].empty()) {
            collisions++;
        }

        table[index].push_back({key, value});
    }

    string search(string key) {
        int index = hashFunction(key);

        for (Record r : table[index]) {
            if (r.key == key) {
                return r.value;
            }
        }

        return "No encontrado";
    }

    int getCollisions() {
        return collisions;
    }

    double loadFactor(int totalElements) {
        return static_cast<double>(totalElements) / size;
    }
};

class HashTableLinear {
private:
    int size;
    vector<Record> table;
    vector<bool> occupied;
    int collisions;

public:
    HashTableLinear(int tableSize) {
        size = tableSize;
        table.resize(size);
        occupied.resize(size, false);
        collisions = 0;
    }

    int hashFunction(string key) {
        unsigned long hash = 0;

        for (char c : key) {
            hash = hash * 31 + c;
        }

        return hash % size;
    }

    void insert(string key, string value) {
        int index = hashFunction(key);

        while (occupied[index]) {
            collisions++;
            index = (index + 1) % size;
        }

        table[index] = {key, value};
        occupied[index] = true;
    }

    string search(string key) {
        int index = hashFunction(key);
        int start = index;

        while (occupied[index]) {
            if (table[index].key == key) {
                return table[index].value;
            }

            index = (index + 1) % size;

            if (index == start) {
                break;
            }
        }

        return "No encontrado";
    }

    int getCollisions() {
        return collisions;
    }

    double loadFactor(int totalElements) {
        return static_cast<double>(totalElements) / size;
    }
};

vector<string> readKeysFromCSV(string filename, int limit) {
    vector<string> keys;
    ifstream file(filename);
    string line;

    getline(file, line);

    while (getline(file, line) && keys.size() < limit) {
        stringstream ss(line);
        string column;
        vector<string> columns;

        while (getline(ss, column, ',')) {
            columns.push_back(column);
        }

        if (!columns.empty()) {
            keys.push_back(columns[0]);
        }
    }

    return keys;
}

int main() {
    string filename = "online_retail.csv";
    int limit = 10000;
    int tableSize = 20011;

    vector<string> keys = readKeysFromCSV(filename, limit);

    HashTableChaining hashChain(tableSize);
    HashTableLinear hashLinear(tableSize);

    auto start = chrono::high_resolution_clock::now();

    for (string key : keys) {
        hashChain.insert(key, key);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> insertChainTime = end - start;

    start = chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000 && i < keys.size(); i++) {
        hashChain.search(keys[i]);
    }

    end = chrono::high_resolution_clock::now();
    chrono::duration<double> searchChainTime = end - start;

    start = chrono::high_resolution_clock::now();

    for (string key : keys) {
        hashLinear.insert(key, key);
    }

    end = chrono::high_resolution_clock::now();
    chrono::duration<double> insertLinearTime = end - start;

    start = chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000 && i < keys.size(); i++) {
        hashLinear.search(keys[i]);
    }

    end = chrono::high_resolution_clock::now();
    chrono::duration<double> searchLinearTime = end - start;

    cout << "RESULTADOS DEL EXPERIMENTO" << endl;
    cout << "---------------------------" << endl;

    cout << "Encadenamiento separado:" << endl;
    cout << "Tiempo de insercion: " 
         << insertChainTime.count() 
         << " segundos" << endl;

    cout << "Tiempo de busqueda: " 
         << searchChainTime.count() 
         << " segundos" << endl;

    cout << "Colisiones: " 
         << hashChain.getCollisions() 
         << endl;

    cout << "Factor de carga: " 
         << hashChain.loadFactor(keys.size()) 
         << endl;

    cout << endl;

    cout << "Sondeo lineal:" << endl;
    cout << "Tiempo de insercion: " 
         << insertLinearTime.count() 
         << " segundos" << endl;

    cout << "Tiempo de busqueda: " 
         << searchLinearTime.count() 
         << " segundos" << endl;

    cout << "Colisiones: " 
         << hashLinear.getCollisions() 
         << endl;

    cout << "Factor de carga: " 
         << hashLinear.loadFactor(keys.size()) 
         << endl;

    return 0;
}