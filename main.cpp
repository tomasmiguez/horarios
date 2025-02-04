#include <iostream> // ??
#include <vector>   //vector
#include <fstream>  //ifstream, ofstream
#include <string>   //stoi
#include <sstream>  //sstream
#include <random>  //rand, srand
#include <cmath>   //pow
#include <iomanip>  //setw
#include <algorithm>//find
#include <chrono>


using namespace std;

static mt19937 GEN(chrono::steady_clock::now().time_since_epoch().count());

struct Modulo {
    unsigned int _duracion;
    unsigned int _idMateria;
    vector<unsigned int> _idGrupos;

    unsigned int _pos;
};

class Horario {

    unsigned int _cantGrupos;
    unsigned int _cantModulos;

    vector<Modulo> _modulos;

    unsigned int _dias;
    unsigned int _posPorDia;

public:
    explicit Horario(const string& filename);

    void shufflePos();
    void mover(unsigned int modulo, int dPos);

    unsigned int getCantPos() {return _dias*_posPorDia;}
    unsigned int getCantGrupos() {return _cantGrupos;}
    unsigned int getCantModulos() {return _cantModulos;}

    unsigned int getCosto();

    void printHorario();
};

Horario::Horario(const string& filename) {

    _modulos = vector<Modulo>();

    ifstream data(filename);

    if(!data.is_open()) cout << "Archivo " << filename << " no encontrado." << endl;

    string auxS;

    getline(data, auxS, ' ');
    _dias = stoi(auxS);
    getline(data, auxS);
    _posPorDia = stoi(auxS);

    getline(data, auxS, ' ');
    _cantGrupos = stoi(auxS);
    getline(data, auxS);
    _cantModulos = stoi(auxS);

    while(getline(data, auxS)) {
        Modulo auxM;
        auxM._idGrupos = vector<unsigned int>();

        std::istringstream ss(auxS);

        ss >> auxM._idMateria;
        ss >> auxM._duracion;
        unsigned int id;
        while (ss >> id)
            auxM._idGrupos.push_back(id);

        uniform_int_distribution<> dis(0, getCantPos()-1);
        auxM._pos = dis(GEN);

       _modulos.push_back(auxM);
    }
}

void Horario::mover(unsigned int modulo, int dPos) {
    if(modulo < _cantModulos) {
        (_modulos[modulo])._pos = ((_modulos[modulo])._pos + dPos) % getCantPos();
    }
}

unsigned int Horario::getCosto() { // Medio cara, pensar mejorge
    unsigned long long costo = 0;

    vector< vector< unsigned int > > coliciones(getCantPos(), vector<unsigned int>(_cantGrupos, 0));
    for(const auto& modActual : _modulos) {
        for(auto grupoActual : modActual._idGrupos) {
            ((coliciones[modActual._pos])[grupoActual])++;
        }
    }

    for(const auto& posActual : coliciones) {
        for(auto cantColiGrupoActual : posActual) {
            if(cantColiGrupoActual > 1)
                costo += 10; //pow(1.1, cantColiGrupoActual) * 1000;
        }
    }

    return costo;
}

void Horario::printHorario() {
    cout << "Costo: " << getCosto() << endl;
    cout << setw(15) << "Dias: " << _dias << setw(15) << "#Pos: " << _posPorDia << endl;
    cout << setw(15) << "#Grupos: " << _cantGrupos << setw(15) << "#Modulos: " << _cantModulos << endl;
    cout << endl << endl;

    /*cout << "Modulos: \n";
    for(auto moduloActual : _modulos) {
        cout << "Id: " << moduloActual._idMateria << " Duracion: " << moduloActual._duracion << endl;
    }
    cout << endl << endl;*/

    for(unsigned int grupoActual = 0; grupoActual < _cantGrupos; grupoActual++) {
        cout << "*-*-*-*-Horario grupo " << grupoActual << "-*-*-*-*\n";
        for(unsigned int diaActual = 0; diaActual < _dias; diaActual++) {
            cout << setw(15) << diaActual;
        }
        cout << endl;
        for(unsigned int posActual = 0; posActual < _posPorDia; posActual++) {
            for(unsigned int diaActual = 0; diaActual < _dias; diaActual++) {
                    cout << setw(15) << "|";
                for(auto moduloActual : _modulos) {
                    if((find(moduloActual._idGrupos.begin(), moduloActual._idGrupos.end(), grupoActual) != moduloActual._idGrupos.end()) &&
                       (moduloActual._pos == diaActual*_posPorDia + posActual)) {
                        cout << moduloActual._idMateria << " ";
                    }
                }
                cout << "|";
            }
            cout << endl;
        }
    }
}

double P(Horario h, Horario hNew, double T) {
    if(hNew.getCosto() < h.getCosto()) return 1;
    else return exp(-((double)(hNew.getCosto() - h.getCosto()))/T);
}

Horario simAnnealing(Horario h, double T0, unsigned long long k) {

    double T = T0;
    for(unsigned long long i = 0; i < k; i++) {
        T = T0 * (1-((double)i/(double)k));//T0/log(i+1);

        Horario hNew(h);
        uniform_int_distribution<> disInt(0, hNew.getCantModulos()-1);
        uniform_real_distribution<> disFlt(0, 1);
        if(disFlt(GEN) < 0.5) hNew.mover(disInt(GEN), 1);
        else hNew.mover(disInt(GEN), -1);

        if(P(h, hNew, T) >= disFlt(GEN)) {
            h = hNew;
        }
    }

    return h;
}

int main()
{
    auto start = std::chrono::system_clock::now();

    GEN.discard(1000);

    Horario test("test3.in");

    test = simAnnealing(test, 100, 100000);

    test.printHorario();

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Tiempo de ejecucion: " << elapsed.count() << " ms.\n";
    return 0;
}
