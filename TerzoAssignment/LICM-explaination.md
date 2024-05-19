# Loop Invariant Code Motion

## Problema

La **LICM** è un'ottimizzazione dei compilatori che sposta il codice invariante dai cicli alle loro estensioni, riducendo così il numero di volte che il codice viene eseguito all'interno del ciclo. Ciò può migliorare significativamente le prestazioni del programma, poiché il codice invariante viene eseguito solo una volta anziché ripetutamente all'interno del ciclo.

## File di test 

Come esempio per provare il nostro passo è stato utilizzato il seguente `file.c`

```c
#include <stdio.h>

void foo(int c, int z) {
  int a = 9, h, m = 0, n = 0, q, r = 0, y = 0;

LOOP:
  z = z + 1;
  y = c + 3;
  q = c + 7;
  if (z < 5) {
    a = a + 2;
    h = c + 3;
  } else {
    a = a - 1;
    h = c + 4;
    if (z >= 10) {
      goto EXIT;
    }
  }
  m = y + 7;
  n = h + 2;
  y = c + 7;
  r = q + 5;
  goto LOOP;
EXIT:
  printf("%d,%d,%d,%d,%d,%d,%d,%d\n", a, h, m, n, q, r, y, z);
}

int main() {
  foo(0, 4);
  foo(0, 12);
  return 0;
}
```

Questo file è stato convertito nella sua rappresentazione IR ottimizzata utilizzando **mem2reg**, che lo porta nella forma **SSA** (Static Single Assignment). In questa forma ogni variabile è assegnata esattamente una sola volta e deve essere sempre definita prima di essere usata. Questo ci aiuterà a garantire che diverse condizioni per la code motion siano già rispettate, senza effettuare ulteriori controlli sulle definzioni e gli usi delle variabili.

Nel file di esempio abbiamo che le istruzioni **loop-invariant** sono:
- ```y = c + 3```, _c_ è fuori dal loop e _3_ è costante
- ```q = c + 7```, _c_ è fuori dal loop e _7_ è costante
- ```h = c + 3```, _c_ è fuori dal loop e _3_ è costante
- ```h = c + 4```, _c_ è fuori dal loop e _4_ è costante
- ```m = y + 7```, _y_ è loop-invariant
- ```y = c + 7```, _c_ è fuori dal loop e _7_ è costante
- ```r = q + 5```, _q_ è loop-invariant

Tra queste istruzioni solo alcune sono candidate alla **code-motion**, ovvero:
- ```y = c + 3```, domina le uscite del loop e il suo blocco domina tutti i blocchi che ne fanno uso
- ```q = c + 7```, stesso motivo della precedente istruzione
- ```y = c + 7```, non domina tutte le uscite, ma non è usata all'uscita e non viene usata nel loop

La code-motion sposta queste istruzioni al preheader se il loop è in forma normalizzata.
