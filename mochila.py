"""
Problema de la mochila: algoritmos voraces vs programacion dinamica.
Tarea 7_2 - Algoritmos Voraces - Problemas
Autor: Emerson Aldair Flores Flores (cod. 240439) - UNAP
"""
import random
import time
from dataclasses import dataclass


@dataclass
class Elemento:
    nombre: str
    peso: int
    valor: int
    cantidad: float = 0.0  # fraccion cargada (0..1)

    @property
    def relacion(self) -> float:
        return self.valor / self.peso


# ---------------------------------------------------------------
# 1) VORAZ - mochila FRACCIONADA (optimo)  O(n log n)
# ---------------------------------------------------------------
def voraz_fraccionada(capacidad, elementos):
    items = sorted(elementos, key=lambda e: e.relacion, reverse=True)
    disponible = capacidad
    total = 0.0
    sel = []
    for e in items:
        if disponible <= 0:
            break
        x = min(1.0, disponible / e.peso)
        sel.append((e.nombre, x))
        total += x * e.valor
        disponible -= x * e.peso
    return total, sel


# ---------------------------------------------------------------
# 2) VORAZ - mochila 0/1 (heuristica, NO siempre optima)  O(n log n)
#    criterio: 'relacion' (valor/peso), 'valor' o 'peso' (menor primero)
# ---------------------------------------------------------------
def voraz_01(capacidad, elementos, criterio="relacion"):
    if criterio == "relacion":
        clave = lambda e: -e.relacion
    elif criterio == "valor":
        clave = lambda e: -e.valor
    else:
        clave = lambda e: e.peso
    disponible = capacidad
    total = 0
    sel = []
    for e in sorted(elementos, key=clave):
        if e.peso <= disponible:  # funcion de factibilidad
            sel.append(e.nombre)
            total += e.valor
            disponible -= e.peso
    return total, sel


# ---------------------------------------------------------------
# 3) PROGRAMACION DINAMICA - mochila 0/1 (optimo)  O(n*P) tiempo y espacio
#    m[i][w] = mejor valor usando los primeros i elementos con capacidad w
#    m[i][w] = max( m[i-1][w], m[i-1][w-p_i] + v_i )   si p_i <= w
# ---------------------------------------------------------------
def dinamica_01(capacidad, elementos):
    n = len(elementos)
    m = [[0] * (capacidad + 1) for _ in range(n + 1)]
    for i in range(1, n + 1):
        p, v = elementos[i - 1].peso, elementos[i - 1].valor
        for w in range(capacidad + 1):
            m[i][w] = m[i - 1][w]
            if p <= w and m[i - 1][w - p] + v > m[i][w]:
                m[i][w] = m[i - 1][w - p] + v
    # reconstruccion de la solucion
    sel, w = [], capacidad
    for i in range(n, 0, -1):
        if m[i][w] != m[i - 1][w]:
            sel.append(elementos[i - 1].nombre)
            w -= elementos[i - 1].peso
    sel.reverse()
    return m[n][capacidad], sel, m


def imprimir_tabla(m, capacidad, elementos):
    print("      " + " ".join(f"{w:3d}" for w in range(capacidad + 1)))
    for i, fila in enumerate(m):
        etiqueta = "  -  " if i == 0 else f"  {elementos[i-1].nombre}  "
        print(etiqueta + " ".join(f"{x:3d}" for x in fila))


def main():
    # ---- Ejemplo del libro (capitulo 21) ----
    capacidad = 40
    elementos = [Elemento("A", 36, 50), Elemento("B", 30, 48), Elemento("C", 20, 30)]

    print("=== Ejemplo del libro: capacidad =", capacidad, "kg ===")
    v, s = voraz_fraccionada(capacidad, elementos)
    print(f"Voraz fraccionada     : valor = {v:.2f}  seleccion = {s}")
    for crit in ("relacion", "valor", "peso"):
        v, s = voraz_01(capacidad, elementos, crit)
        print(f"Voraz 0/1 ({crit:8s}) : valor = {v}  seleccion = {s}")
    v, s, m = dinamica_01(capacidad, elementos)
    print(f"Prog. dinamica 0/1    : valor = {v}  seleccion = {s}")

    # ---- Ejemplo pequeno para mostrar la tabla DP ----
    print("\n=== Tabla DP (capacidad 10) ===")
    pequeno = [Elemento("A", 5, 10), Elemento("B", 4, 40), Elemento("C", 6, 30), Elemento("D", 3, 50)]
    v, s, m = dinamica_01(10, pequeno)
    imprimir_tabla(m, 10, pequeno)
    print("valor optimo =", v, "seleccion =", s)

    # ---- Experimento: frecuencia de fallo del voraz 0/1 ----
    random.seed(2026)
    pruebas, fallos, brecha = 2000, 0, 0.0
    for _ in range(pruebas):
        n = random.randint(3, 8)
        its = [Elemento(chr(65 + k), random.randint(1, 20), random.randint(1, 60)) for k in range(n)]
        cap = random.randint(10, 50)
        vg, _ = voraz_01(cap, its, "relacion")
        vd, _, _ = dinamica_01(cap, its)
        if vg < vd:
            fallos += 1
            brecha += (vd - vg) / vd
    print(f"\n=== Experimento ({pruebas} instancias aleatorias) ===")
    print(f"El voraz 0/1 por relacion v/p fallo en {fallos} ({100*fallos/pruebas:.1f}%)")
    print(f"Perdida relativa media en los fallos: {100*brecha/max(fallos,1):.1f}%")

    # ---- Experimento: tiempos ----
    print("\n=== Tiempos (ms) ===")
    print(f"{'n':>6} {'P':>7} {'voraz':>10} {'dinamica':>12}")
    for n, P in [(100, 1000), (200, 2000), (400, 4000)]:
        its = [Elemento(str(k), random.randint(1, 50), random.randint(1, 500)) for k in range(n)]
        t0 = time.perf_counter(); voraz_01(P, its); t1 = time.perf_counter()
        dinamica_01(P, its); t2 = time.perf_counter()
        print(f"{n:6d} {P:7d} {1000*(t1-t0):10.3f} {1000*(t2-t1):12.3f}")


if __name__ == "__main__":
    main()
