const express = require('express');
const multer = require('multer');
const { execFile } = require('child_process');
const fs = require('fs');
const path = require('path');

const app = express();
const port = 3000;

// Configuración de Multer para la carga de archivos
const upload = multer({ dest: 'uploads/' });

// Ruta para la API que recibe parámetros
app.post('/api/simplex/params', express.json(), (req, res) => {
    const { variables, restricciones } = req.body;

    // Crear el archivo con los parámetros recibidos
    const fileContent = `${variables.length} ${restricciones.length}\n`;
    const simplexInput = variables.join(' ') + '\n' + restricciones.map(r => r.join(' ')).join('\n') + '\n';

    // Guardar los parámetros en un archivo temporal
    fs.writeFileSync('input_params.txt', fileContent + simplexInput);

    // Ejecutar el método Simplex en C++
    execFile('./simplex', ['input_params.txt'], (error, stdout, stderr) => {
        if (error) {
            console.error(`Error al ejecutar Simplex: ${stderr}`);
            res.status(500).json({ error: 'Error al ejecutar Simplex' });
            return;
        }

        // Guardar la salida del método simplex en un archivo de solución
        fs.writeFileSync('solucion_params.txt', stdout);

        // Enviar la solución al cliente
        res.json({ solution: stdout });
    });
});



// Ruta para la API que recibe un archivo
app.post('/api/simplex/file', upload.single('file'), (req, res) => {
    if (!req.file) {
        return res.status(400).send('No se ha subido ningún archivo.');
    }

    // Leer el archivo subido
    const filePath = req.file.path;
    fs.readFile(filePath, 'utf8', (err, data) => {
        if (err) {
            return res.status(500).send('Error al leer el archivo.');
        }

        // Convertir los datos de la matriz separados por comas a un formato adecuado
        const lines = data.split('\n');
        const matrix = lines.map(line => line.trim().split(',').map(Number));

        // Formatear la entrada para el método simplex
        const numVariables = matrix[0].length;
        const numRestricciones = matrix.length;
        let simplexInput = `${numVariables} ${numRestricciones}\n`;

        simplexInput += matrix.map(row => row.join(' ')).join('\n') + '\n';

        // Guardar los datos en un archivo temporal
        const inputFilePath = 'input_file.txt';
        fs.writeFileSync(inputFilePath, simplexInput);

        // Ejecutar el programa Simplex en C++
        execFile('./simplex', [inputFilePath], (error, stdout, stderr) => {
            if (error) {
                console.error(`Error al ejecutar Simplex: ${stderr}`);
                return res.status(500).json({ error: 'Error al ejecutar Simplex' });
            }

            // Escribir la solución en un archivo de salida
            const outputFilePath = 'solution.txt';
            fs.writeFileSync(outputFilePath, stdout);

            // Enviar la solución al cliente
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
