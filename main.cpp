#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <cstdlib>
#include <math.h>

using namespace std;

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
    Horario(string filename);

    void shufflePos();

    unsigned int getCantPos() {return _dias*_posPorDia;}

    unsigned int getCosto();
};

Horario::Horario(string filename) : _dias(5), _posPorDia(14) {
    _modulos = vector<Modulo>();

    ifstream data(filename);
    string auxS;

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

        auxM._pos = (rand() % getCantPos());
    }
}

unsigned int Horario::getCosto() { // Medio cara, pensar mejorge
    unsigned long long costo = 0;

    vector< vector< unsigned int > > coliciones(getCantPos(), vector<unsigned int>(_cantGrupos, 0));
    for(auto modActual : _modulos) {
        for(auto grupoActual : modActual._idGrupos) {
            ((coliciones[modActual._pos])[grupoActual])++;
        }
    }

    for(auto posActual : coliciones) {
        for(auto cantColiGrupoActual : posActual) {
            costo += pow(1.1, cantColiGrupoActual) * 1000;
        }
    }

    return costo;
}

int main()
{
    srand(time(NULL));



    return 0;
}
