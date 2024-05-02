# Soluzione Esercizio 2
## Traccia
Per il programma riportato, già diviso in Basic Blocks, occorre:
 - Derivare una formalizzazione per il framework di Dataflow Analysis
 - Popolare una tabella con le iterazioni dell’algoritmo iterativo di soluzione del problema

Un’espressione è **very busy** in un punto p se, indipendentemente dal percorso preso da p, l’espressione viene usata prima che uno dei suoi operandi venga definito.

## Soluzione primo punto
|  						|Dataflow Problem 1 (VBE)									  |
|-----------------------|-------------------------------------------------------------|
|Domain   				|Sets of expressions										  |
|Direction				|Forward: $out[b] = f_b(in[b]), in[b] = (\wedge out[pred(b)])$|
|Transfer Function		|$f_b(x)=gen_b\cup(x-kill)$									  |
|Meet Operation			|$(\cap)$													  |
|Boundary Condition		|$out[entry]=(\{\})$										  |
|Initial Interior Points|$out[b]=U$													  |

## Soluzione secondo punto

|Nome BB|Contenuto|Iterazione 1 - in[b]|Iterazione 1 - out[b]|Iterazione 2 - in[b]|Iterazione2 - out[b] 
|-------|---------|--------------------|---------------------|--------------------|-------------------|
|BB1	|entry	  |
|BB2	|(a!=b)?  |
|BB3	|x=b-a    |
|BB4	|x=a-b    |
|BB5	|y=b-a    |
|BB6	|a=0	  |
|BB7	|x=a-b	  |
|BB8	|exit	  |



