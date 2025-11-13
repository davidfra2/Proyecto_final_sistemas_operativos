# Simulador de Kernel - Sistemas Operativos

## Descripción General

Este proyecto implementa un **simulador de kernel educativo** que demuestra los conceptos fundamentales de un sistema operativo moderno. Incluye simulación de:

- **Planificación de procesos** (Round Robin)
- **Gestión de memoria virtual** (LRU - Least Recently Used)
- **Sincronización** (Productor-Consumidor con semáforos)
- **Entrada/Salida** (Primitivas de sincronización)

Es una herramienta didáctica para entender cómo los sistemas operativos modernos (Linux, Windows) manejan recursos y procesos.

---

## Arquitectura del Kernel

```
┌─────────────────────────────────────────────────┐
│         CLI - Interfaz de Usuario               │
│              (main.cpp)                         │
└────────────────┬────────────────────────────────┘
                 │
    ┌────────────┼────────────┬──────────────┐
    ▼            ▼            ▼              ▼
┌────────┐  ┌────────┐  ┌────────┐  ┌─────────┐
│  CPU   │  │ MEMORY │  │   I/O  │  │ KERNEL  │
│Module  │  │ Module │  │ Module │  │ (sync)  │
└────────┘  └────────┘  └────────┘  └─────────┘
    │            │            │              │
    ▼            ▼            ▼              ▼
┌────────────────────────────────────────────────┐
│    Sistema de Archivos Virtual (Módulos)       │
└────────────────────────────────────────────────┘
```

### Estructura de Directorios

```
proyecto_final_sistemas_operativos/
├── cli/
│   └── main.cpp              # Interfaz CLI del simulador
├── modules/
│   ├── cpu/
│   │   ├── pcb.h             # Process Control Block
│   │   └── rr_scheduler.cpp  # Planificador Round Robin
│   ├── mem/
│   │   ├── lru.h             # Algoritmo LRU
│   │   ├── lru.cpp           # Implementación LRU
│   │   └── fifo.cpp          # Algoritmo FIFO (referencia)
│   └── io/
│       └── sync.cpp          # Sincronización Productor-Consumidor
├── kernel.cpp                # Kernel monolítico (referencia)
└── README.md                 # Este archivo
```

---

## Componentes Principales

### 1. **Módulo CPU - Planificación Round Robin**

#### Descripción
Implementa un planificador **Round Robin (RR)** que ejecuta procesos en turnos equitativos. Cada proceso recibe una cantidad fija de tiempo de CPU (quantum).

#### Archivo: `modules/cpu/rr_scheduler.cpp`

#### Algoritmo RR Explicado

```
┌─────────────────────────────────────────┐
│    Cola de Listos (Ready Queue)         │
│  [P1] [P2] [P3] [P4] ...               │
└─────────────────────────────────────────┘
         ↓
    ┌────────────┐
    │   Running  │ ← Quantum = 4 ticks
    │     P1     │
    └────────────┘
         ↓
    Ejecuta 4 ticks:
    - Si termina → TERMINATED
    - Si no → va al final de la cola (PREEMPTION)
```

**Pasos del Algoritmo:**

1. **Selección**: Tomar el primer proceso de la cola de listos
2. **Ejecución**: Ejecutar durante `quantum` ticks (por defecto 4)
3. **Decisión**:
   - Si `bursts_remaining == 0` → Terminar proceso
   - Si `quantum_used >= quantum` → Preemptar y encolar nuevamente
   - Si termina antes → Terminar proceso

**Complejidad**:
- Tiempo: **O(n)** por tick (donde n = número de procesos)
- Espacio: **O(n)** para almacenar PCBs

**Métricas Calculadas**:
```cpp
struct PCB {
    int pid;                 // ID único
    State state;             // READY, RUNNING, BLOCKED, TERMINATED
    int bursts_remaining;    // Tiempo de CPU restante
    int arrival_time;        // Momento de llegada
    int start_time;          // Cuando inició ejecución
    int finish_time;         // Cuando terminó
};
```

---

### 2. **Módulo MEMORIA - LRU (Least Recently Used)**

#### Descripción
Implementa el algoritmo **LRU** para gestión de memoria virtual. Cuando hay fallo de página, evicta la página menos recientemente utilizada.

#### Archivo: `modules/mem/lru.cpp`

#### Algoritmo LRU Explicado

```
Acceso a página 1:
┌─────────────────────┐
│ Memory [1]          │  (1 acceso)

Acceso a página 2:
┌─────────────────────┐
│ Memory [2 | 1]      │  (2 es más reciente)

Acceso a página 3:
┌─────────────────────┐
│ Memory [3 | 2 | 1]  │  (3 frames llenos)

Acceso a página 1 (hit):
┌─────────────────────┐
│ Memory [1 | 3 | 2]  │  (mover 1 al frente)

Acceso a página 4 (miss):
┌─────────────────────┐
│ Memory [4 | 1 | 3]  │  (evictar 2, menos usado)
```

**Estructura de Datos**:
```cpp
std::list<int> memory;                              // Orden LRU
std::unordered_map<int, std::list<int>::iterator> 
    page_map;                                       // Búsqueda O(1)
```

**Pasos del Algoritmo**:

1. **Hit** (página en memoria):
   - Mover página al frente (más reciente)
   - Sin fallo de página

2. **Miss** (página no en memoria):
   - Si memoria llena → Evictar página del final (menos reciente)
   - Insertar nueva página al frente
   - Incrementar contador de fallos

**Complejidad**:
- Tiempo: **O(1)** por acceso (con unordered_map)
- Espacio: **O(f)** donde f = número de frames

**Comparativa con otros algoritmos**:

| Algoritmo | Fallo | Descripción |
|-----------|-------|-------------|
| **LRU** | Bajo | Usa histórico reciente (implementado) |
| **FIFO** | Medio | Evicta la página más antigua |
| **Óptimo** | Mejor | Evicta la que se usará más tarde (no es realizable) |

---

### 3. **Módulo I/O - Sincronización**

#### Descripción
Implementa el problema clásico de **Productor-Consumidor** usando:
- **Mutex**: Exclusión mutua
- **Condition Variables**: Señalización entre threads
- **Buffer**: Cola sincronizada

#### Archivo: `modules/io/sync.cpp`

#### Patrón Explicado

```
┌──────────────┐
│  Productor   │
│  (Thread 1)  │
└───────┬──────┘
        │ produce_n(n)
        ▼
    ┌────────────┐
    │  Buffer    │  (max 5 items)
    │  (Mutex)   │
    └────────────┘
        ▲
        │ consume_n(n)
┌───────┴──────┐
│  Consumidor  │
│  (Thread 2)  │
└──────────────┘
```

**Sincronización Implementada**:

```cpp
// PRODUCTOR espera a que haya espacio
cv.wait(lk, [] { return (int)buffer.size() < BUFFER_MAX; });
buffer.push(item);
cv.notify_all();

// CONSUMIDOR espera a que haya datos
cv.wait(lk, [] { return !buffer.empty(); });
int item = buffer.front(); 
buffer.pop();
cv.notify_all();
```

**Problemas Evitados**:
- ✅ Race condition (mutex)
- ✅ Deadlock (condition variables)
- ✅ Buffer overflow (control de tamaño)
- ✅ Consumo de CPU (wait en condición)

---

## Guía de Uso

### Compilación

```bash
# En Windows con g++
g++ -o kernel.exe cli/main.cpp modules/cpu/rr_scheduler.cpp modules/mem/lru.cpp modules/io/sync.cpp -std=c++11 -pthread

# En Linux
g++ -o kernel cli/main.cpp modules/cpu/rr_scheduler.cpp modules/mem/lru.cpp modules/io/sync.cpp -std=c++11 -pthread
```

### Ejecución Interactiva

```bash
$ ./kernel
=== Simulador de Kernel (kernel-sim) ===

Comandos disponibles:
  new                - Crear nuevo proceso
  ps                 - Listar procesos
  kill <pid>         - Terminar proceso
  run <n>            - Simular n procesos aleatorios
  tick               - Avanza un quantum
  mem <trace>        - Prueba LRU con traza
  produce <n>        - Productor produce n items
  consume <n>        - Consumidor consume n items
  help               - Mostrar ayuda
  exit               - Salir

kernel> new
Proceso creado PID=1

kernel> ps
PID	State	Bursts
1	READY	3

kernel> tick
[tick 1] Running PID 1, bursts left 2, quantum 1/4

kernel> mem 1,2,3,2,4
Access page 1
Frames: 1 	 Faults: 1
Access page 2
Frames: 2 1 	 Faults: 2
Access page 3
Frames: 3 2 1 	 Faults: 3
Access page 2
Frames: 2 3 1 	 Faults: 3
Access page 4
Frames: 4 2 3 	 Faults: 4
Fallos totales: 4

kernel> exit
Saliendo...
```

### Ejemplos de Uso

#### 1. Simulación Round Robin

```bash
kernel> run 3
CREATED 1
[tick 1] Running PID 1, bursts left 4, quantum 1/4
CREATED 2
[tick 2] Running PID 1, bursts left 3, quantum 2/4
CREATED 3
[tick 3] Running PID 1, bursts left 2, quantum 3/4
[tick 4] Running PID 1, bursts left 1, quantum 4/4
PID 1 preempted, back to ready.
[tick 5] Running PID 2, bursts left 2, quantum 1/4
...
```

#### 2. Simulación de Memoria LRU

```bash
kernel> mem 1,2,3,4,1,2,5,1,2,3
Access page 1
Frames: 1 	 Faults: 1
Access page 2
Frames: 2 1 	 Faults: 2
Access page 3
Frames: 3 2 1 	 Faults: 3
Access page 4
Frames: 4 3 2 	 Faults: 4
Access page 1
Frames: 1 4 3 	 Faults: 4
Access page 2
Frames: 2 1 4 	 Faults: 5
Access page 5
Frames: 5 2 1 	 Faults: 6
Access page 1
Frames: 1 5 2 	 Faults: 6
Access page 2
Frames: 2 1 5 	 Faults: 6
Access page 3
Frames: 3 2 1 	 Faults: 7

Fallos totales: 7
```

#### 3. Sincronización Productor-Consumidor

```bash
kernel> produce 5
Produced item 0, buffer size 1
Produced item 1, buffer size 2
Produced item 2, buffer size 3
Produced item 3, buffer size 4
Produced item 4, buffer size 5

kernel> consume 3
Consumed item 0, buffer size 4
Consumed item 1, buffer size 3
Consumed item 2, buffer size 2
```

---

## Análisis de Algoritmos

### Comparativa de Planificadores

| Métrica | Round Robin | FCFS | SJF |
|---------|------------|------|-----|
| **Equidad** | ✅ Alta | ✅ Muy alta | ❌ Baja |
| **Starvation** | ❌ No | ❌ No | ✅ Posible |
| **Overhead** | ⚠️ Medio | ✅ Bajo | ⚠️ Medio |
| **Promedio Espera** | ⚠️ Depende Q | ❌ Alto | ✅ Bajo |

### Impacto del Quantum en RR

```
Quantum = 1 (muy pequeño):
├─ Mucho context switch
├─ Tiempo de CPU bien distribuido
└─ Overhead alto

Quantum = 4 (nuestro caso):
├─ Balance entre equidad y overhead
├─ Recomendado para sistemas generales
└─ Buen balance overhead/rendimiento

Quantum = 100 (muy grande):
├─ Menos context switch
├─ Similar a FCFS
└─ Algunos procesos esperan demasiado
```
## Complejidad Computacional

| Operación | Complejidad | Notas |
|-----------|------------|-------|
| create_process() | O(1) | Enqueue en ready queue |
| kill_process(pid) | O(n) | Búsqueda en vector all |
| tick() | O(1) amortizado | Con unordered_map |
| LRU::run_trace() | O(p) | p = número de páginas |
| LRU::access() | O(1) | Con unordered_map |
| Productor::produce() | O(n) | n = items a producir |

---

## Conceptos Teóricos

### Process Control Block (PCB)

El PCB es la estructura fundamental que OS mantiene para cada proceso:

```cpp
struct PCB {
    int pid;                      // Identificador único
    ProcessState state;           // Estado actual
    int bursts_remaining;         // CPU time restante
    int arrival_time;             // Cuándo llegó
    int start_time;               // Cuándo inició ejecución
    int finish_time;              // Cuándo terminó
    std::vector<int> pages;       // Páginas que usa
};
```

### Context Switch

Proceso de cambio entre procesos:

```
Tiempo: 3 → 4
├─ Guardar estado de P1 (registros, PC)
├─ Actualizar PCB de P1
├─ Cargar estado de P2
├─ Actualizar PCB de P2
└─ Reanudar ejecución de P2
```

---

## Referencias

### Libros Recomendados
- **Operating System Concepts** - Silberschatz, Galvin, Gagne
- **Modern Operating Systems** - Andrew Tanenbaum
- **Linux Kernel Development** - Robert Love

### Conceptos Clave
- [Wikipedia: Scheduling Algorithm](https://en.wikipedia.org/wiki/Scheduling_(computing))
- [Virtual Memory](https://en.wikipedia.org/wiki/Virtual_memory)
- [Producer-Consumer Problem](https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem)

---
