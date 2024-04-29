# Soluzione Esercizio 2

In un CFG diciamo che un nodo X domina un altro nodo Y se il nodo X appare in ogni percorso
del grafo che porta dal blocco ENTRY al blocco Y.

Per definizione un nodo domina sé stesso 

## Soluzione primo punto

|                               | Dataflow Problem 2 (DA)                                                    |
|-------------------------------|------------------------------------------------------------------------------|
| **Dominio**                   | nodi --> basic blocks                       |
| **Direction**                 | Forward: $out[b] = f_b(in[b]), in[b] = (\wedge out[pred(b)])$                |
| **Transfer Function**         | $f_b(x)=gen_b(\cup(x))$                                           |
| **Meet Operation**            | $(\cap)$                                                                   |
| **Boundary Condition**        | $out[entry]=(\{\})$                                                         |
| **Initial Interior Points**   | $out[b]=U$                                                                |

## Soluzione secondo punto


| OUT[B] | A | B | C | D | E | F | G |
|-------|---|---|---|---|---|---|---|
| ENTRY | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| A     | 1 | 0 | 0 | 0 | 0 | 0 | 0 |
| B     | 1 | 1 | 0 | 0 | 0 | 0 | 0 |
| C     | 1 | 0 | 1 | 0 | 0 | 0 | 0 |
| D     | 1 | 0 | 1 | 1 | 0 | 0 | 0 |
| E     | 1 | 0 | 1 | 0 | 1 | 0 | 0 |
| F     | 1 | 0 | 1 | 0 | 0 | 1 | 0 |
| G     | 1 | 0 | 0 | 0 | 0 | 0 | 1 |             |
