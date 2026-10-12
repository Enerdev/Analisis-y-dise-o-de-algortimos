// Problema de la mochila: algoritmos voraces vs programacion dinamica.
// Tarea 7_2 - Algoritmos Voraces - Problemas
// Autor: Emerson Aldair Flores Flores (cod. 240439) - UNAP
// Compilar: g++ -std=c++17 -O2 mochila.cpp -o mochila
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Elemento {
    string nombre;
    int peso;
    int valor;
    double relacion() const { return (double)valor / peso; }
};

// 1) VORAZ - mochila fraccionada (optimo) O(n log n)
double vorazFraccionada(int capacidad, vector<Elemento> v, vector<pair<string,double>>& sel) {
    sort(v.begin(), v.end(), [](const Elemento& a, const Elemento& b) {
        return a.relacion() > b.relacion();
    });
    double disponible = capacidad, total = 0;
    for (auto& e : v) {
        if (disponible <= 0) break;
        double x = min(1.0, disponible / e.peso);
        sel.push_back({e.nombre, x});
        total += x * e.valor;
        disponible -= x * e.peso;
    }
    return total;
}

// 2) VORAZ - mochila 0/1 (heuristica) O(n log n)
// criterio: 0 = valor/peso, 1 = valor, 2 = peso (menor primero)
int vorazCeroUno(int capacidad, vector<Elemento> v, int criterio, vector<string>& sel) {
    sort(v.begin(), v.end(), [criterio](const Elemento& a, const Elemento& b) {
        if (criterio == 0) return a.relacion() > b.relacion();
        if (criterio == 1) return a.valor > b.valor;
        return a.peso < b.peso;
    });
    int disponible = capacidad, total = 0;
    for (auto& e : v) {
        if (e.peso <= disponible) {          // funcion de factibilidad
            sel.push_back(e.nombre);
            total += e.valor;
            disponible -= e.peso;
        }
    }
    return total;
}

// 3) PROGRAMACION DINAMICA - mochila 0/1 (optimo) O(n*P)
// m[i][w] = max(m[i-1][w], m[i-1][w-p_i] + v_i)
int dinamicaCeroUno(int capacidad, const vector<Elemento>& v, vector<string>& sel,
                    vector<vector<int>>* tabla = nullptr) {
    int n = v.size();
    vector<vector<int>> m(n + 1, vector<int>(capacidad + 1, 0));
    for (int i = 1; i <= n; i++) {
        int p = v[i - 1].peso, val = v[i - 1].valor;
        for (int w = 0; w <= capacidad; w++) {
            m[i][w] = m[i - 1][w];
            if (p <= w) m[i][w] = max(m[i][w], m[i - 1][w - p] + val);
        }
    }
    int w = capacidad;                       // reconstruccion
    for (int i = n; i >= 1; i--) {
        if (m[i][w] != m[i - 1][w]) {
            sel.push_back(v[i - 1].nombre);
            w -= v[i - 1].peso;
        }
    }
    reverse(sel.begin(), sel.end());
    int res = m[n][capacidad];
    if (tabla) *tabla = m;
    return res;
}

void mostrar(const vector<string>& s) {
    cout << "{";
    for (size_t i = 0; i < s.size(); i++) cout << (i ? ", " : "") << s[i];
    cout << "}";
}

int main() {
    int capacidad = 40;
    vector<Elemento> el = {{"A", 36, 50}, {"B", 30, 48}, {"C", 20, 30}};

    cout << "=== Ejemplo del libro: capacidad = " << capacidad << " kg ===\n";
    vector<pair<string,double>> sf;
    double vf = vorazFraccionada(capacidad, el, sf);
    cout << fixed << setprecision(2) << "Voraz fraccionada     : valor = " << vf << "  ";
    for (auto& p : sf) cout << p.first << "=" << p.second << " ";
    cout << "\n";

    const char* nombres[] = {"relacion", "valor   ", "peso    "};
    for (int c = 0; c < 3; c++) {
        vector<string> s;
        int v = vorazCeroUno(capacidad, el, c, s);
        cout << "Voraz 0/1 (" << nombres[c] << ") : valor = " << v << "  seleccion = ";
        mostrar(s); cout << "\n";
    }
    vector<string> sd;
    int vd = dinamicaCeroUno(capacidad, el, sd);
    cout << "Prog. dinamica 0/1    : valor = " << vd << "  seleccion = ";
    mostrar(sd); cout << "\n";

    // Experimento: frecuencia de fallo del voraz 0/1
    srand(2026);
    int pruebas = 2000, fallos = 0;
    for (int t = 0; t < pruebas; t++) {
        int n = 3 + rand() % 6;
        vector<Elemento> its;
        for (int k = 0; k < n; k++)
            its.push_back({string(1, 'A' + k), 1 + rand() % 20, 1 + rand() % 60});
        int cap = 10 + rand() % 41;
        vector<string> s1, s2;
        if (vorazCeroUno(cap, its, 0, s1) < dinamicaCeroUno(cap, its, s2)) fallos++;
    }
    cout << "\n=== Experimento (" << pruebas << " instancias) ===\n";
    cout << "El voraz 0/1 fallo en " << fallos << " (" << 100.0 * fallos / pruebas << "%)\n";

    // Tiempos
    cout << "\n=== Tiempos (ms) ===\n";
    cout << setw(6) << "n" << setw(8) << "P" << setw(12) << "voraz" << setw(12) << "dinamica\n";
    int casos[3][2] = {{100, 1000}, {200, 2000}, {400, 4000}};
    for (auto& c : casos) {
        vector<Elemento> its;
        for (int k = 0; k < c[0]; k++) its.push_back({to_string(k), 1 + rand() % 50, 1 + rand() % 500});
        vector<string> s;
        auto t0 = chrono::high_resolution_clock::now();
        vorazCeroUno(c[1], its, 0, s);
        auto t1 = chrono::high_resolution_clock::now();
        s.clear();
        dinamicaCeroUno(c[1], its, s);
        auto t2 = chrono::high_resolution_clock::now();
        cout << setw(6) << c[0] << setw(8) << c[1] << setw(12)
             << chrono::duration<double, milli>(t1 - t0).count() << setw(12)
             << chrono::duration<double, milli>(t2 - t1).count() << "\n";
    }
    return 0;
}
