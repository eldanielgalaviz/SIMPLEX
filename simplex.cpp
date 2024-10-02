#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

class Simplex {
private:
    vector<vector<double>> tableau;
    int num_variables;
    int num_restricciones;

public: //abrimos el archvo generado por la api
    Simplex(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error al abrir el archivo." << endl;
            exit(1);
        }

        // extraemos del archivo lss variables y restricciones
        file >> num_variables >> num_restricciones;
        tableau.resize(num_restricciones + 1, vector<double>(num_variables + num_restricciones + 1));

        // extraemos la función objeto
        for (int i = 0; i < num_variables; i++) {
            file >> tableau[0][i];
            tableau[0][i] *= -1;
        }

        // leemos las restricciones
        for (int i = 1; i <= num_restricciones; i++) {
            for (int j = 0; j < num_variables; j++) {
                file >> tableau[i][j];
            }
            file >> tableau[i][num_variables + num_restricciones]; // lado derecho despues del igual
            tableau[i][num_variables + i - 1] = 1;  // variable de holgura
        }

        file.close();
    }

    void mostrarTabla() {
        for (int i = 0; i <= num_restricciones; i++) {
            for (int j = 0; j <= num_variables + num_restricciones; j++) {
                cout << setw(10) << tableau[i][j] << " ";
            }
            cout << endl;
        }
    }

    int columnaPivote() {
        int pivot_col = 0;
        for (int i = 1; i < num_variables + num_restricciones; i++) {
            if (tableau[0][i] < tableau[0][pivot_col]) {
                pivot_col = i;
            }
        }
        return pivot_col;
    }

    int filaPivote(int pivot_col) {
        int pivot_row = -1;
        double min_ratio = INFINITY;
        for (int i = 1; i <= num_restricciones; i++) {
            if (tableau[i][pivot_col] > 0) {
                double ratio = tableau[i][num_variables + num_restricciones] / tableau[i][pivot_col];
                if (ratio < min_ratio) {
                    min_ratio = ratio;
                    pivot_row = i;
                }
            }
        }
        return pivot_row;
    }

    void pivotear(int pivot_row, int pivot_col) {
        double pivot_value = tableau[pivot_row][pivot_col];

        for (int i = 0; i <= num_variables + num_restricciones; i++) {
            tableau[pivot_row][i] /= pivot_value;
        }

        for (int i = 0; i <= num_restricciones; i++) {
            if (i != pivot_row) {
                double factor = tableau[i][pivot_col];
                for (int j = 0; j <= num_variables + num_restricciones; j++) {
                    tableau[i][j] -= factor * tableau[pivot_row][j];
                }
            }
        }
    }

    bool esOptima() {
        for (int i = 0; i < num_variables + num_restricciones; i++) {
            if (tableau[0][i] < 0) {
                return false;
            }
        }
        return true;
    }

    void obtenerSolucion() {
        cout << "\nSolucion optima encontrada:\n";
        for (int i = 0; i < num_variables; i++) {
            bool es_variable_basica = false;
            double valor_variable = 0;
            for (int j = 1; j <= num_restricciones; j++) {
                if (tableau[j][i] == 1) {
                    es_variable_basica = true;
                    valor_variable = tableau[j][num_variables + num_restricciones];
                    break;
                }
            }
            cout << "x" << i + 1 << " = " << (es_variable_basica ? valor_variable : 0) << endl;
        }
        cout << "Valor optimo de Z: " << tableau[0][num_variables + num_restricciones] << endl;
    }

    void resolverSimplex() {
        while (!esOptima()) {
            int pivot_col = columnaPivote();
            int pivot_row = filaPivote(pivot_col);
            if (pivot_row == -1) {
                cout << "No hay solucion optima (problema no acotado)." << endl;
                return;
            }
            pivotear(pivot_row, pivot_col);
        }
        obtenerSolucion();
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <archivo de entrada>" << endl;
        return 1;
    }

    Simplex simplex(argv[1]);
    simplex.resolverSimplex();
    return 0;
}
