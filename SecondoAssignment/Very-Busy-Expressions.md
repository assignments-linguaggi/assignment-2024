
# Soluzione Esercizio 1
## Traccia
<img style="width:20rem" src="./imgs/Screenshot 2024-04-29 alle 15.21.01.png" alt="">
Per il programma riportato, già diviso in Basic Blocks, occorre:

 - Derivare una formalizzazione per il framework di Dataflow Analysis
 - Popolare una tabella con le iterazioni dell’algoritmo iterativo di soluzione del problema

Un’espressione è **very busy** in un punto p se, indipendentemente dal percorso preso da p, l’espressione viene usata prima che uno dei suoi operandi venga definito.

## Soluzione primo punto
|  						               |Dataflow Problem 1 (VBE)									                            |
|-----------------------|-------------------------------------------------------------|
|Domain   				          |insieme di espressioni									                                |
|Direction				          |Indietro: $`in[b] = f_b(out[b]), out[b] = (\wedge in[next(b)])`$|
|Transfer Function	    	|$f_b(x)=gen_b\cup(out[b]-kill_b)$								                 	 |
|Meet Operation			      |$`(\cap)`$													                                        |
|Boundary Condition	   	|$`in[exit]=\emptyset$									                              	  |
|Initial Interior Points|$in[b]=U$													                                      |

## Soluzione secondo punto

Saranno necessarie due iterazioni, in quanto cambiano gli out[b]

| &nbsp; | in[b] | out[b] |
| ------ | ----- | ------ |
| BB1 (entry) | {(a!=b), (b-a)} | in[BB2]|
| BB2 | {(a!=b), (b-a)} | in[BB3] $`\cap`$ in[BB5] = {(b-a)} |
| BB3 | {(b-a), (a-b)} | in[BB4] |
| BB4 | {(a-b)} | in[BB8] |
| BB5 | {(b-a)} | in[BB6] |
| BB6 | $`\emptyset`$ | in[BB7] |
| BB7 | {(a-b)} | in[exit] |
| BB8 (exit) | $`\emptyset`$ | $`\emptyset`$ |

| &nbsp; | in[b] | out[b] |
| ------ | ----- | ------ |
| BB1 (entry) | {(a!=b), (b-a)} | in[BB2]|
| BB2 | {(a!=b), (b-a)} | in[BB3] $`\cap`$ in[BB5] = {(b-a)} |
| BB3 | {(b-a), (a-b)} | in[BB4] |
| BB4 | {(a-b)} | in[BB8] |
| BB5 | {(b-a)} | in[BB6] |
| BB6 | $`\emptyset`$ | in[BB7] |
| BB7 | {(a-b)} | in[exit] |
| BB8 (exit) | $`\emptyset`$ | $`\emptyset`$ |
