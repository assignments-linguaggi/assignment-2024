# Soluzione Esercizio 2
## Traccia
<img style="width:20rem" src="./imgs/Screenshot 2024-04-29 alle 15.21.01.png" alt="">
Per il programma riportato, già diviso in Basic Blocks, occorre:

 - Derivare una formalizzazione per il framework di Dataflow Analysis
 - Popolare una tabella con le iterazioni dell’algoritmo iterativo di soluzione del problema

Un’espressione è **very busy** in un punto p se, indipendentemente dal percorso preso da p, l’espressione viene usata prima che uno dei suoi operandi venga definito.

## Soluzione primo punto
|  						               |Dataflow Problem 1 (VBE)									                            |
|-----------------------|-------------------------------------------------------------|
|Domain   				          |Sets of expressions										                                |
|Direction				          |Forward: $out[b] = f_b(in[b]), in[b] = (\wedge out[pred(b)])$|
|Transfer Function	    	|$f_b(x)=gen_b\cup(out[b]-kill(b))$								                 	 |
|Meet Operation			      |$(\cap)$													                                        |
|Boundary Condition	   	|$out[entry]=(\{\})$									                              	  |
|Initial Interior Points|$out[b]=U$													                                      |

## Soluzione secondo punto

|Nome BB|Contenuto|Iterazione 1 - in[b]	  |Iterazione 1 - out[b]|
|-------|---------|-----------------------|---------------------|
|BB1	|entry	      |{}					                |{}				              	|
|BB2	|(a!=b)?     |out[BB1]				           |{}					              |
|BB3	|x=b-a       |out[BB2]				           |{(b-a)}				          |
|BB4	|x=a-b       |out[BB3] 			           |{(b-a}, (a-b)}	     	|
|BB5	|y=b-a       |out[BB2]				           |{(b-a)}				          |
|BB6	|a=0	        |out[BB5]				           |{(b-a)}		         		 |
|BB7	|x=a-b	      |out[BB6]				           |{(b-a), (a-b)}	     	|
|BB8	|exit	       |out[BB4]$\cup$ out[BB7]|{(b-a), (a-b)}	     	|



