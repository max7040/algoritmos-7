// tabla_hash2.cpp
// Analisis de Tablas Hash - Comercio Electronico
// Dataset: Online Retail II (Kaggle)
// Metodos: Encadenamiento | Sondeo Lineal | Doble Hashing | unordered_map

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <chrono>
#include <string>
#include <unordered_map>
#include <iomanip>

using namespace std;

// ------------------------------------------------------------
// ESTRUCTURA BASE
// ------------------------------------------------------------

struct Record {
    string key;
    string value;
};

// ------------------------------------------------------------
// FUNCION HASH 1 - Polinomio de Horner, base 31
// ------------------------------------------------------------

unsigned int hashFunction(const string& key, int tableSize) {
    unsigned long hash = 0;
    for (char c : key)
        hash = hash * 31 + static_cast<unsigned char>(c);
    return hash % tableSize;
}

// ------------------------------------------------------------
// FUNCION HASH 2 - Para doble hashing, base 37
// Nunca devuelve 0 para garantizar cobertura total
// ------------------------------------------------------------

unsigned int hashFunction2(const string& key, int prime) {
    unsigned long hash = 0;
    for (char c : key)
        hash = hash * 37 + static_cast<unsigned char>(c);
    unsigned int step = prime - (hash % prime);
    return (step == 0) ? 1 : step;
}

// ------------------------------------------------------------
// METODO 1: ENCADENAMIENTO SEPARADO
// Cada bucket es una lista enlazada de Records
// ------------------------------------------------------------

class HashTableChaining {
private:
    int size;
    vector<list<Record>> table;
    int collisions;

public:
    HashTableChaining(int tableSize) : size(tableSize), collisions(0) {
        table.resize(size);
    }

    void insert(const string& key, const string& value) {
        int index = hashFunction(key, size);
        if (!table[index].empty())
            collisions++;
        table[index].push_back({key, value});
    }

    string search(const string& key) {
        int index = hashFunction(key, size);
        for (const Record& r : table[index])
            if (r.key == key) return r.value;
        return "No encontrado";
    }

    int    getCollisions() const { return collisions; }
    double loadFactor(int n) const { return static_cast<double>(n) / size; }
};

// ------------------------------------------------------------
// METODO 2: SONDEO LINEAL
// Al colisionar: index = (index + 1) % size
// ------------------------------------------------------------

class HashTableLinear {
private:
    int size;
    vector<Record> table;
    vector<bool> occupied;
    int primaryCollisions;
    int probingSteps;

public:
    HashTableLinear(int tableSize) : size(tableSize), primaryCollisions(0), probingSteps(0) {
        table.resize(size);
        occupied.resize(size, false);
    }

    void insert(const string& key, const string& value) {
        int index = hashFunction(key, size);
        bool first = true;
        while (occupied[index]) {
            if (first) { primaryCollisions++; first = false; }
            probingSteps++;
            index = (index + 1) % size;
        }
        table[index]    = {key, value};
        occupied[index] = true;
    }

    string search(const string& key) {
        int index = hashFunction(key, size);
        int start = index;
        while (occupied[index]) {
            if (table[index].key == key) return table[index].value;
            index = (index + 1) % size;
            if (index == start) break;
        }
        return "No encontrado";
    }

    int    getCollisions()   const { return primaryCollisions; }
    int    getProbingSteps() const { return probingSteps; }
    double loadFactor(int n) const { return static_cast<double>(n) / size; }
};

// ------------------------------------------------------------
// METODO 3: DOBLE HASHING
// index_i = (h1 + i * h2) % size
// ------------------------------------------------------------

class HashTableDouble {
private:
    int size;
    int prime;
    vector<Record> table;
    vector<bool> occupied;
    int primaryCollisions;
    int probingSteps;

public:
    HashTableDouble(int tableSize, int prime2)
        : size(tableSize), prime(prime2), primaryCollisions(0), probingSteps(0) {
        table.resize(size);
        occupied.resize(size, false);
    }

    void insert(const string& key, const string& value) {
        int h1    = hashFunction(key, size);
        int h2    = hashFunction2(key, prime);
        int index = h1;
        bool first = true;
        while (occupied[index]) {
            if (first) { primaryCollisions++; first = false; }
            probingSteps++;
            index = (h1 + probingSteps * h2) % size;
        }
        table[index]    = {key, value};
        occupied[index] = true;
    }

    string search(const string& key) {
        int h1    = hashFunction(key, size);
        int h2    = hashFunction2(key, prime);
        int index = h1;
        int steps = 0;
        while (occupied[index] && steps < size) {
            if (table[index].key == key) return table[index].value;
            steps++;
            index = (h1 + steps * h2) % size;
        }
        return "No encontrado";
    }

    int    getCollisions()   const { return primaryCollisions; }
    int    getProbingSteps() const { return probingSteps; }
    double loadFactor(int n) const { return static_cast<double>(n) / size; }
};

// ------------------------------------------------------------
// BUSQUEDA AUTOMATICA DEL CSV
// ------------------------------------------------------------

string findCSV(const string& filename) {
    vector<string> candidates = {
        filename,
        "..\\" + filename,
        "../"  + filename,
        "algoritmos-7\\" + filename,
        "algoritmos-7/"  + filename,
        ".\\" + filename,
        "./"  + filename
    };
    for (const string& p : candidates) {
        ifstream test(p);
        if (test.is_open()) {
            cout << "Archivo encontrado en: " << p << "\n";
            return p;
        }
    }
    return "";
}

// ------------------------------------------------------------
// LECTURA DEL CSV
// Columna 0 = Invoice (clave), Columna 2 = Description (valor)
// ------------------------------------------------------------

vector<pair<string,string>> readRecordsFromCSV(const string& filename, int limit) {
    vector<pair<string,string>> records;

    string path = findCSV(filename);
    if (path.empty()) {
        cerr << "\nERROR: No se encontro '" << filename << "'.\n";
        cerr << "Soluciones:\n";
        cerr << "  1) Copia el CSV a la misma carpeta que el .exe\n";
        cerr << "  2) Ejecuta desde la carpeta algoritmos-7:\n";
        cerr << "     cd C:\\ruta\\algoritmos-7\n";
        cerr << "     .\\tabla_hash2.exe\n\n";
        return records;
    }

    ifstream file(path);
    string line;
    getline(file, line);

    while (getline(file, line) && (int)records.size() < limit) {
        stringstream ss(line);
        string col;
        vector<string> cols;
        while (getline(ss, col, ','))
            cols.push_back(col);
        if (cols.size() >= 3 && !cols[0].empty())
            records.push_back({cols[0], cols[2]});
    }

    cout << "Registros leidos: " << records.size() << "\n";
    return records;
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------

int main() {
    const string filename   = "online_retail_II.csv";
    const int    LIMIT      = 10000;
    const int    TABLE_SIZE = 20011;
    const int    PRIME2     = 19937;
    const int    SEARCH_N   = 1000;

    auto records = readRecordsFromCSV(filename, LIMIT);
    if (records.empty()) return 1;

    HashTableChaining            hashChain(TABLE_SIZE);
    HashTableLinear              hashLinear(TABLE_SIZE);
    HashTableDouble              hashDouble(TABLE_SIZE, PRIME2);
    unordered_map<string,string> hashNativo;
    hashNativo.reserve(LIMIT * 2);

    // ---- insercion ----
    auto t0 = chrono::high_resolution_clock::now();
    for (const auto& r : records) hashChain.insert(r.first, r.second);
    auto t1 = chrono::high_resolution_clock::now();
    double insertChain = chrono::duration<double>(t1 - t0).count();

    t0 = chrono::high_resolution_clock::now();
    for (const auto& r : records) hashLinear.insert(r.first, r.second);
    t1 = chrono::high_resolution_clock::now();
    double insertLinear = chrono::duration<double>(t1 - t0).count();

    t0 = chrono::high_resolution_clock::now();
    for (const auto& r : records) hashDouble.insert(r.first, r.second);
    t1 = chrono::high_resolution_clock::now();
    double insertDouble = chrono::duration<double>(t1 - t0).count();

    t0 = chrono::high_resolution_clock::now();
    for (const auto& r : records) hashNativo[r.first] = r.second;
    t1 = chrono::high_resolution_clock::now();
    double insertNativo = chrono::duration<double>(t1 - t0).count();

    // ---- busqueda ----
    t0 = chrono::high_resolution_clock::now();
    for (int i = 0; i < SEARCH_N && i < (int)records.size(); i++)
        hashChain.search(records[i].first);
    t1 = chrono::high_resolution_clock::now();
    double searchChain = chrono::duration<double>(t1 - t0).count();

    t0 = chrono::high_resolution_clock::now();
    for (int i = 0; i < SEARCH_N && i < (int)records.size(); i++)
        hashLinear.search(records[i].first);
    t1 = chrono::high_resolution_clock::now();
    double searchLinear = chrono::duration<double>(t1 - t0).count();

    t0 = chrono::high_resolution_clock::now();
    for (int i = 0; i < SEARCH_N && i < (int)records.size(); i++)
        hashDouble.search(records[i].first);
    t1 = chrono::high_resolution_clock::now();
    double searchDouble = chrono::duration<double>(t1 - t0).count();

    t0 = chrono::high_resolution_clock::now();
    for (int i = 0; i < SEARCH_N && i < (int)records.size(); i++)
        hashNativo.count(records[i].first);
    t1 = chrono::high_resolution_clock::now();
    double searchNativo = chrono::duration<double>(t1 - t0).count();

    // ---- tabla comparativa ----
    int N = (int)records.size();

    cout << "\n=== RESULTADOS DEL EXPERIMENTO - TABLAS HASH ===\n";
    cout << "Dataset: Online Retail II (Kaggle) | Registros: " << N << "\n\n";

    cout << left
         << setw(28) << "Metrica"
         << setw(16) << "Encadenamiento"
         << setw(16) << "Sondeo Lineal"
         << setw(16) << "Doble Hashing"
         << setw(14) << "unordered_map"
         << "\n";
    cout << string(90, '-') << "\n";

    cout << left
         << setw(28) << "Insercion (ms)"
         << setw(16) << fixed << setprecision(4) << insertChain  * 1000
         << setw(16) << insertLinear * 1000
         << setw(16) << insertDouble * 1000
         << setw(14) << insertNativo * 1000
         << "\n";

    cout << left
         << setw(28) << "Busqueda 1000 claves (ms)"
         << setw(16) << searchChain  * 1000
         << setw(16) << searchLinear * 1000
         << setw(16) << searchDouble * 1000
         << setw(14) << searchNativo * 1000
         << "\n";

    cout << left
         << setw(28) << "Colisiones primarias"
         << setw(16) << hashChain.getCollisions()
         << setw(16) << hashLinear.getCollisions()
         << setw(16) << hashDouble.getCollisions()
         << setw(14) << "N/A"
         << "\n";

    cout << left
         << setw(28) << "Pasos de probing"
         << setw(16) << "N/A"
         << setw(16) << hashLinear.getProbingSteps()
         << setw(16) << hashDouble.getProbingSteps()
         << setw(14) << "N/A"
         << "\n";

    cout << left
         << setw(28) << "Factor de carga"
         << setw(16) << fixed << setprecision(4) << hashChain.loadFactor(N)
         << setw(16) << hashLinear.loadFactor(N)
         << setw(16) << hashDouble.loadFactor(N)
         << setw(14) << "auto"
         << "\n";

    cout << string(90, '-') << "\n";

    // ---- verificacion de correctitud ----
    cout << "\n=== VERIFICACION DE CORRECTITUD ===\n\n";
    for (int idx : {0, 100, 500, 1000, 5000}) {
        if (idx >= N) continue;
        const string& k = records[idx].first;
        string r1 = hashChain.search(k);
        string r2 = hashLinear.search(k);
        string r3 = hashDouble.search(k);
        string r4 = hashNativo.count(k) ? hashNativo[k] : "No encontrado";
        cout << "Clave [" << setw(7) << k << "]  "
             << "Chain="  << (r1 != "No encontrado" ? "OK" : "FAIL") << "  "
             << "Linear=" << (r2 != "No encontrado" ? "OK" : "FAIL") << "  "
             << "Double=" << (r3 != "No encontrado" ? "OK" : "FAIL") << "  "
             << "Nativo=" << (r4 != "No encontrado" ? "OK" : "FAIL") << "\n";
    }

    // ---- factor de carga vs tamano de tabla ----
    cout << "\n=== FACTOR DE CARGA VS TAMANO DE TABLA (N = " << N << ") ===\n\n";
    cout << left << setw(16) << "Tamano tabla"
                 << setw(14) << "Factor carga"
                 << "Clasificacion\n";
    cout << string(50, '-') << "\n";

    for (int sz : {10007, 15013, 20011, 30011, 40009}) {
        double lf = static_cast<double>(N) / sz;
        string clasif;
        if      (lf < 0.50) clasif = "Optimo    (< 0.50)";
        else if (lf < 0.70) clasif = "Bueno     (< 0.70)";
        else if (lf < 0.85) clasif = "Aceptable (< 0.85)";
        else                 clasif = "Critico   (>= 0.85)";

        cout << left << setw(16) << sz
                     << setw(14) << fixed << setprecision(4) << lf
                     << clasif << "\n";
    }

    cout << "\n[Programa finalizado correctamente]\n";
    return 0;
}