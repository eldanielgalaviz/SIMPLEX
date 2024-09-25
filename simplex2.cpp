#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>

using namespace std;

class Simplex {
private:
    vector<vector<double>> tableau; // Tabla simplex
    int num_variables;              // Número de variables
    int num_restrictions;           // Número de restricciones

public:
    // Constructor
    Simplex(int variables, int restrictions) {
        num_variables = variables;
        num_restrictions = restrictions;
        tableau.resize(restrictions + 1, vector<double>(variables + restrictions + 1, 0.0));
    }

    // Función para ingresar la función objetivo
    void ingresarFuncionObjetivo() {
        cout << "Ingrese los coeficientes de la funcion objetivo (max Z):\n";
        for (int i = 0; i < num_variables; i++) {
            cin >> tableau[0][i];
            tableau[0][i] *= -1;  // Convertir a forma estándar (minimización)
        }
    }

    // Función para ingresar las restricciones
    void ingresarRestricciones() {
        for (int i = 1; i <= num_restrictions; i++) {
            cout << "Ingrese los coeficientes de la restriccion " << i << " (incluya el lado derecho de la desigualdad):\n";
            for (int j = 0; j < num_variables; j++) {
                cin >> tableau[i][j];
            }
            char tipo_restriccion;
            cout << "Ingrese el tipo de restriccion (1: <=, 2: >=, 3: =): ";
            cin >> tipo_restriccion;

            if (tipo_restriccion == '1') {
                tableau[i][num_variables + i - 1] = 1;  // Variable de holgura
            } else if (tipo_restriccion == '2') {
                tableau[i][num_variables + i - 1] = -1; // Variable de exceso
                tableau[0][num_variables + i - 1] = 0;  // Coeficiente en Z es 0
            } else if (tipo_restriccion == '3') {
                tableau[i][num_variables + i - 1] = 1;  // Variable de holgura
                tableau.push_back(vector<double>(num_variables + num_restrictions + 1, 0));
                tableau[tableau.size() - 1][0] = 1;  // Z = variable de holgura
                tableau[tableau.size() - 1][num_variables + i - 1] = -1; // Introducir igual
                tableau[tableau.size() - 1][num_variables + num_restrictions] = tableau[i][num_variables + num_restrictions]; // lado derecho
                num_restrictions++; // Incrementar el número de restricciones
            }
            cin >> tableau[i][num_variables + num_restrictions];  // Lado derecho
        }
    }

    // Función para mostrar la tabla simplex
    void mostrarTabla() {
        cout << "\nTabla Simplex:\n";
        for (int i = 0; i <= num_restrictions; i++) {
            for (int j = 0; j <= num_variables + num_restrictions; j++) {
                cout << setw(10) << fixed << setprecision(2) << tableau[i][j] << " ";
            }
            cout << endl;
        }
    }

    // Función para encontrar la columna pivote (la más negativa en la fila Z)
    int columnaPivote() {
        int pivot_col = 0;
        for (int i = 1; i < num_variables + num_restrictions; i++) {
            if (tableau[0][i] < tableau[0][pivot_col]) {
                pivot_col = i;
            }
        }
        return pivot_col;
    }

    // Función para encontrar la fila pivote
    int filaPivote(int pivot_col) {
        int pivot_row = -1;
        double min_ratio = INFINITY;

        for (int i = 1; i <= num_restrictions; i++) {
            if (tableau[i][pivot_col] > 0) {
                double ratio = tableau[i][num_variables + num_restrictions] / tableau[i][pivot_col];
                if (ratio < min_ratio) {
                    min_ratio = ratio;
                    pivot_row = i;
                }
            }
        }

        return pivot_row;
    }

    // Función para realizar la operación de pivotaje
    void pivotear(int pivot_row, int pivot_col) {
        double pivot_value = tableau[pivot_row][pivot_col];

        // Dividir la fila pivote por el valor pivote
        for (int i = 0; i <= num_variables + num_restrictions; i++) {
            tableau[pivot_row][i] /= pivot_value;
        }

        // Realizar operaciones en las otras filas
        for (int i = 0; i <= num_restrictions; i++) {
            if (i != pivot_row) {
                double factor = tableau[i][pivot_col];
                for (int j = 0; j <= num_variables + num_restrictions; j++) {
                    tableau[i][j] -= factor * tableau[pivot_row][j];
                }
            }
        }
    }

    // Función para verificar si la solución es óptima
    bool esOptima() {
        for (int i = 0; i < num_variables + num_restrictions; i++) {
            if (tableau[0][i] < 0) {
                return false;
            }
        }
        return true;
    }

    // Función para obtener la solución óptima
    void obtenerSolucion() {
        cout << "\nSolucion optima encontrada:\n";
        for (int i = 0; i < num_variables; i++) {
            bool es_variable_basica = false;
            double valor_variable = 0;
            for (int j = 1; j <= num_restrictions; j++) {
                if (tableau[j][i] == 1) {
                    es_variable_basica = true;
                    valor_variable = tableau[j][num_variables + num_restrictions];
                    break;
                }
            }
            if (es_variable_basica) {
                cout << "x" << i + 1 << " = " << valor_variable << endl;
            } else {
                cout << "x" << i + 1 << " = 0" << endl;
            }
        }
        cout << "Valor optimo de Z: " << tableau[0][num_variables + num_restrictions] << endl;
    }

    // Función principal del método simplex
    void resolverSimplex() {
        mostrarTabla();

        while (!esOptima()) {
            int pivot_col = columnaPivote();
            int pivot_row = filaPivote(pivot_col);

            if (pivot_row == -1) {
                cout << "No hay solucion optima (problema no acotado)." << endl;
                return;
            }

            cout << "\nPivoteando en la fila " << pivot_row << " y columna " << pivot_col << "...\n";
            pivotear(pivot_row, pivot_col);
            mostrarTabla();
        }

        obtenerSolucion();
    }
};

int main() {
    int num_variables, num_restrictions;

    cout << "Ingrese el numero de variables: ";
    cin >> num_variables;
    cout << "Ingrese el numero de restricciones: ";
    cin >> num_restrictions;

    Simplex simplex(num_variables, num_restrictions);

    simplex.ingresarFuncionObjetivo();
    simplex.ingresarRestricciones();

    simplex.resolverSimplex();

    return 0;
}
