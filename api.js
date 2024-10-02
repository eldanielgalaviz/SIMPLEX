const express = require('express');
const multer = require('multer');
const { execFile } = require('child_process');
const fs = require('fs');
const path = require('path');

const app = express();
const port = 3000;

// multer para la carga de archivos
const upload = multer({ dest: 'uploads/' });

// ruta para la api que recibe los valores
app.post('/api/simplex/params', express.json(), (req, res) => {
    const { variables, restricciones } = req.body;

    // crear el archivo 
    // contiene las variables y restricciones
    const fileContent = `${variables.length} ${restricciones.length}\n`;
    const simplexInput = variables.join(' ') + '\n' + restricciones.map(r => r.join(' ')).join('\n') + '\n';

    // euardar los valores de las V y R
    fs.writeFileSync('input_params.txt', fileContent + simplexInput);

    // ejecutar el archivo simplex de c++
    execFile('./simplex', ['input_params.txt'], (error, stdout, stderr) => {
        if (error) {
            console.error(`Error al ejecutar Simplex: ${stderr}`);
            res.status(500).json({ error: 'Error al ejecutar Simplex' });
            return;
        }

        // guardamos la solucion
        fs.writeFileSync('solucion_params.txt', stdout);

        // devolvemos el resultado
        res.json({ solution: stdout });
    });
});



// ruta  API  archivo
app.post('/api/simplex/file', upload.single('file'), (req, res) => {
    if (!req.file) {
        return res.status(400).send('No se ha subido elS archivo.');
    }

    // lee el archivo subido
    const filePath = req.file.path;
    fs.readFile(filePath, 'utf8', (err, data) => {
        if (err) {
            return res.status(500).send('Error al leer el archivo.');
        }

        // convertir los datos de la matriz separados por comas 
        const lines = data.split('\n');
        const matrix = lines.map(line => line.trim().split(',').map(Number));

        // se cuentan las variables y las restricciones para convertir los datos de entrada
        const numVariables = matrix[0].length;
        const numRestricciones = matrix.length;
        let simplexInput = `${numVariables} ${numRestricciones}\n`;

        // separa por filas ys e conviernte cada fila en una se parada por espacion "join('')" y luego se une cada fila con saltos de línea "join('\n')"
        simplexInput += matrix.map(row => row.join(' ')).join('\n') + '\n';

        // guarda los datos en un archivo temporal
        const inputFilePath = 'input_file.txt';
        fs.writeFileSync(inputFilePath, simplexInput);

        // ejecuta el simplex.exe, se pasa el archivo de entrada (inputFilePath) 
        execFile('./simplex', [inputFilePath], (error, stdout, stderr) => {
            if (error) {
                console.error(`Error al ejecutar Simplex: ${stderr}`);
                return res.status(500).json({ error: 'Error al ejecutar Simplex' });
            }

            // solución en un archivo de salida
            const outputFilePath = 'solution.txt';
            fs.writeFileSync(outputFilePath, stdout);

            // soluviom
            res.json({
                mensaje: 'Archivo procesado correctamente',
                solucion: stdout,
                archivoSalida: outputFilePath
            });
        });
    });
});

app.listen(port, () => {
    console.log(`Servidor escuchando en http://localhost:${port}`);
});
