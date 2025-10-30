#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>   
#include "../modules/cpu/pcb.h"
#include "../modules/cpu/rr_scheduler.h"
#include "../modules/mem/lru.h"
#include "../modules/io/sync.h"

using namespace std;

void printHelp() {
    cout << "\nComandos disponibles:" << endl;
    cout << "  new                - Crear nuevo proceso (pid autogenerado)" << endl;
    cout << "  ps                 - Listar procesos" << endl;
    cout << "  kill <pid>         - Terminar proceso" << endl;
    cout << "  run <n>            - Simular n procesos aleatorios (crea y ejecuta)" << endl;
    cout << "  tick               - Avanza un quantum (simulador RR)" << endl;
    cout << "  mem <trace>        - Prueba LRU con traza separada por comas (ej: 1,2,3,2,4)" << endl;
    cout << "  produce <n>        - Productor produce n items (demo sync)" << endl;
    cout << "  consume <n>        - Consumidor consume n items (demo sync)" << endl;
    cout << "  help               - Mostrar ayuda" << endl;
    cout << "  exit               - Salir" << endl;
}

int main(){
    cout << "=== Simulador de Kernel (kernel-sim) ===" << endl;
    printHelp();
    RR_Scheduler rr(4); // quantum = 4 ticks
    LRU memsim(3); // 3 frames por defecto
    SyncDemo sync;

    string line;
    while(true){
        cout << "\nkernel> ";
        if(!getline(cin, line)) break;
        istringstream iss(line);
        string cmd; iss >> cmd;
        if(cmd=="") continue;
        if(cmd=="help"){ printHelp(); continue; }
        if(cmd=="new"){
            int pid = rr.create_process( (rand()%5)+1 ); // rafagas 1..5
            cout << "Proceso creado PID="<<pid<<endl;
            continue;
        }
        if(cmd=="ps"){
            rr.print_processes();
            continue;
        }
        if(cmd=="kill"){
            int pid; if(iss>>pid){ rr.kill_process(pid); } else cout<<"Especifica PID"<<endl;
            continue;
        }
        if(cmd=="tick"){
            rr.tick();
            continue;
        }
        if(cmd=="run"){
            int n; if(iss>>n){
                for(int i=0;i<n;i++){
                    int pid=rr.create_process((rand()%5)+1);
                    cout<<"CREATED "<<pid<<endl;
                    rr.tick();
                }
            } else cout<<"run <n>"<<endl;
            continue;
        }
        if(cmd=="mem"){
            string trace;
            if(getline(iss, trace)){
                // remove spaces
                trace.erase(remove(trace.begin(), trace.end(), ' '), trace.end());
                vector<int> pages;
                string cur;
                for(char c:trace){
                    if(c==','){ if(!cur.empty()) pages.push_back(stoi(cur)); cur.clear(); }
                    else cur.push_back(c);
                }
                if(!cur.empty()) pages.push_back(stoi(cur));
                int faults = memsim.run_trace(pages);
                cout<<"Fallos totales: "<<faults<<endl;
            } else cout<<"mem 1,2,3,2,4"<<endl;
            continue;
        }
        if(cmd=="runmem"){
            // quick demo
            vector<int> trace = {1,2,3,2,4,1,5,2,3,4,1,2};
            cout<<"LRU demo trace: ";
            for(int p:trace) cout<<p<<",";
            cout<<endl;
            cout<<"Fallos: "<<memsim.run_trace(trace)<<endl;
            continue;
        }
        if(cmd=="produce"){
            int n; if(iss>>n){ sync.produce_n(n); } else cout<<"produce <n>"<<endl;
            continue;
        }
        if(cmd=="consume"){
            int n; if(iss>>n){ sync.consume_n(n); } else cout<<"consume <n>"<<endl;
            continue;
        }
        if(cmd=="exit") break;
        cout<<"Comando desconocido. Escribe 'help'."<<endl;
    }

    cout<<"Saliendo..."<<endl;
    return 0;
}
