import pandas as pd
import time
import matplotlib.pyplot as plt


class HashTableChaining:
    def __init__(self, size):
        self.size = size
        self.table = [[] for _ in range(size)]
        self.collisions = 0

    def hash_function(self, key):
        return hash(str(key)) % self.size

    def insert(self, key, value):
        index = self.hash_function(key)

        if len(self.table[index]) > 0:
            self.collisions += 1

        self.table[index].append((key, value))

    def search(self, key):
        index = self.hash_function(key)

        for k, v in self.table[index]:
            if k == key:
                return v

        return None

    def load_factor(self, total_elements):
        return total_elements / self.size


class HashTableLinearProbing:
    def __init__(self, size):
        self.size = size
        self.table = [None] * size
        self.collisions = 0

    def hash_function(self, key):
        return hash(str(key)) % self.size

    def insert(self, key, value):
        index = self.hash_function(key)

        while self.table[index] is not None:
            self.collisions += 1
            index = (index + 1) % self.size

        self.table[index] = (key, value)

    def search(self, key):
        index = self.hash_function(key)
        start = index

        while self.table[index] is not None:
            if self.table[index][0] == key:
                return self.table[index][1]

            index = (index + 1) % self.size

            if index == start:
                break

        return None

    def load_factor(self, total_elements):
        return total_elements / self.size


class HashTableDoubleHashing:
    def __init__(self, size):
        self.size = size
        self.table = [None] * size
        self.collisions = 0

    def hash_function1(self, key):
        return hash(str(key)) % self.size

    def hash_function2(self, key):
        return 1 + (hash(str(key)) % (self.size - 1))

    def insert(self, key, value):
        index = self.hash_function1(key)
        step = self.hash_function2(key)

        while self.table[index] is not None:
            self.collisions += 1
            index = (index + step) % self.size

        self.table[index] = (key, value)

    def search(self, key):
        index = self.hash_function1(key)
        step = self.hash_function2(key)
        start = index

        while self.table[index] is not None:
            if self.table[index][0] == key:
                return self.table[index][1]

            index = (index + step) % self.size

            if index == start:
                break

        return None

    def load_factor(self, total_elements):
        return total_elements / self.size


# Cargar dataset descargado desde Kaggle
df = pd.read_csv("online_retail.csv")

# Limpieza básica de datos
df = df.dropna()

# Selección de claves
keys = df["Invoice"].astype(str).head(10000).tolist()

table_size = 20011


# Prueba con encadenamiento separado
hash_chain = HashTableChaining(table_size)

start_time = time.time()
for key in keys:
    hash_chain.insert(key, {"Invoice": key})
insert_time_chain = time.time() - start_time

start_time = time.time()
for key in keys[:1000]:
    hash_chain.search(key)
search_time_chain = time.time() - start_time


# Prueba con sondeo lineal
hash_linear = HashTableLinearProbing(table_size)

start_time = time.time()
for key in keys:
    hash_linear.insert(key, {"Invoice": key})
insert_time_linear = time.time() - start_time

start_time = time.time()
for key in keys[:1000]:
    hash_linear.search(key)
search_time_linear = time.time() - start_time


# Prueba con doble hashing
hash_double = HashTableDoubleHashing(table_size)

start_time = time.time()
for key in keys:
    hash_double.insert(key, {"Invoice": key})
insert_time_double = time.time() - start_time

start_time = time.time()
for key in keys[:1000]:
    hash_double.search(key)
search_time_double = time.time() - start_time


# Comparación con diccionario nativo de Python
native_dict = {}

start_time = time.time()
for key in keys:
    native_dict[key] = {"Invoice": key}
insert_time_dict = time.time() - start_time

start_time = time.time()
for key in keys[:1000]:
    native_dict.get(key)
search_time_dict = time.time() - start_time


# Tabla comparativa de resultados
results = pd.DataFrame({
    "Metodo": [
        "Encadenamiento",
        "Sondeo lineal",
        "Doble hashing",
        "Diccionario Python"
    ],
    "Tiempo insercion": [
        insert_time_chain,
        insert_time_linear,
        insert_time_double,
        insert_time_dict
    ],
    "Tiempo busqueda": [
        search_time_chain,
        search_time_linear,
        search_time_double,
        search_time_dict
    ],
    "Colisiones": [
        hash_chain.collisions,
        hash_linear.collisions,
        hash_double.collisions,
        0
    ],
    "Factor de carga": [
        hash_chain.load_factor(len(keys)),
        hash_linear.load_factor(len(keys)),
        hash_double.load_factor(len(keys)),
        len(keys) / table_size
    ]
})

print(results)


# Gráfico de tiempo de inserción
plt.figure(figsize=(8, 5))
plt.bar(results["Metodo"], results["Tiempo insercion"])
plt.title("Comparacion de tiempo de insercion")
plt.xlabel("Metodo")
plt.ylabel("Tiempo en segundos")
plt.xticks(rotation=20)
plt.tight_layout()
plt.show()


# Gráfico de tiempo de búsqueda
plt.figure(figsize=(8, 5))
plt.bar(results["Metodo"], results["Tiempo busqueda"])
plt.title("Comparacion de tiempo de busqueda")
plt.xlabel("Metodo")
plt.ylabel("Tiempo en segundos")
plt.xticks(rotation=20)
plt.tight_layout()
plt.show()


# Gráfico de colisiones
plt.figure(figsize=(8, 5))
plt.bar(results["Metodo"], results["Colisiones"])
plt.title("Comparacion de colisiones")
plt.xlabel("Metodo")
plt.ylabel("Numero de colisiones")
plt.xticks(rotation=20)
plt.tight_layout()
plt.show()