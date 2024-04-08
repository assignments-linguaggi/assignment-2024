# Utilizzo del Passo LLVM LocalOpts

Per utilizzare il passo LocalOpts che abbiamo implementato, occorre seguire le seguenti istruzioni:

## Aggiunta dei File Necessari

1. Nella directory `$ROOT/SRC/llvm-project-17.0.6.src/llvm/include/llvm/Transforms/Utils/`, assicurarsi di copiare tutti i file header (`.h`) presenti in questa directory (della repo).

2. Nella directory `$ROOT/SRC/llvm-project-17.0.6.src/llvm/lib/Transforms/Utils/`, copiare tutti i file sorgente (`.cpp`) presenti in questa directory.

3. Copiare il file `CMakeLists.txt` nella stessa directory del secondo punto.

4. Nella directory `$ROOT/SRC/llvm-project-17.0.6.src/llvm/lib/Passes`, copiare il file `PassRegistry.def` e `PassBuilder.cpp`.

## Compilazione del Codice

1. Navigare nella cartella `$ROOT/PrimoAssignment/TEST`.

2. Utilizzare il comando `opt` per applicare il passo localopts al file LLVM IR (`.ll`), e specificare il nome del file di output con l'estensione `.optimized.bc`, come segue: 
    ```bash
    opt -p localopts <nomefile>.ll -o <nomefile>.optimized.bc
    ```

3. Per visualizzare il file LLVM IR ottimizzato, utilizza il comando `llvm-dis` specificando il file `.optimized.bc` e specifica il nome del file di output con l'estensione `.optimized.ll`, nel seguente modo:
    ```bash
    llvm-dis <nomefile>.optimized.bc -o <nomefile>.optimized.ll
    ``` 
