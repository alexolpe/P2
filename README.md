PAV - P2: detección de actividad vocal: Introducción y fundamento teórico(VAD)
============================================
Para empezar, se ha recuperado el fichero .wav que se grabó la clase pasada. Se ha abierto con Wavesurfer y se han etiquetado los segmentos se silencio (S) y voz (V):

![Captura](https://user-images.githubusercontent.com/91128741/159154427-b2ae7279-a6e9-4ecf-a0a7-167eb4025eb2.JPG)

Analizando la tasa de cruces por cero en Wavesurfer:

![Captura2](https://user-images.githubusercontent.com/91128741/159154528-1034c943-8cdf-47b7-85b0-b65e648c0bc4.JPG)

Se observa que, al inicio, la potencia media oscila entre -80 y -60 dB.
Analizando los incrementos de potencia a lo largo de todo el señal, se observa lo siguiente:

![Captura3](https://user-images.githubusercontent.com/91128741/159154560-58fa0eb5-9692-45b8-bd51-df5fc2b63ee0.JPG)

Se observa claramente como en las vocales, al ser sonoras, la potencia incrementa. El resto de consonantes, tienen una potencia inferior. Destacan las “s” sordas que son los segmentos de voz en los que la potencia media es mínima.

En la representación gráfica también se observa que la potencia del silencio no supera los -60 dB. Del mismo modo, la potencia de la voz nunca es inferior a -60 dB. Sin embargo, debido a los sonidos sordos, es difícil encontrar un incremento de potencia suficiente para considerar que se ha pasado de un segmento de voz a uno de silencio y viceversa.

Al analizar el fichero de las transcripciones, se observa lo siguiente:

![Captura4](https://user-images.githubusercontent.com/91128741/159154582-3516c92f-27d8-4565-9fb2-04867418330e.JPG)

En él se da la información sobre los fragmentos temporales etiquetados con S o V.

Implementación del detector de voz (VAD).
----------------------------------------------

Para implementar el detector de voz se ha aplicado un sistema de estados que indican cuando una trama pertenece a voz, cuando pertenece a silencio y cuando no sabemos si es un tramo de voz o de silencio.
Es importante destacar que hay un **estado init** donde se hacen las operaciones pertinentes para escoger unos umbrales de desición lo más óptimos posibles. Se ha declarado una variable llamada **refsil** que es una variable introducida por el terminal y se puede optimizar utilizando:

for alpha1 in $(seq 0 1 10 | tr , .); do for alpha2 in $(seq 0 1 10 | tr , .); do for refsil in $(seq 0 1 10 | tr , .); do echo -n  "$alpha1 " "$alpha2" "refsil";scripts/run_vad.sh $alpha1 $alpha2 $refsil | fgrep TOTAL; done done done| sort -t: -k 2n;

La variable refsil se puede optimizar. Esta variable nos indica cuantas tramas iniciales queremos procesar para obtener nuestro umbral de silencio. Mientras se van procesado las tramas iniciales se va guardando en el struct **vad_data->watts** la suma de las potencias de cada tramo inicial y de las cruces por zero en **vad_data->zcr**. Una vez se han procesado las n tramas iniciales se calculan los umbrales de sonido y se pasa al estado silencio. 
Se utlizan dos umbrales de sonido, uno que guardamos en el struct **vad_data->p1** y otro en **vad_data->p2**. El primer umbral corresponde a la media de la potencia en dB más un cierto valor óptimo introducido por terminal llamado **alpha1**, el segundo umbral corresponde al primer umbral más otro valor óptimo introducido por terminal.

El **estado silencio** decide que si una trama supera el umbral p1 pasa al estado maybe voice y si no lo supera se queda en silencio.

El **estado voice** decide que si una trama no supera el umbral p2 y las cruces por zero son mayores que la media de silencio se pasa al estado maybe silence.

El **estado maybe voice** decide que si la potencia de una trama es menor que la del umbral p1 el estado pasa a silencio, si la potencia de la trama es mayor que el umbral p2 pasa a voice y si hay más de 2 tramas seguidas en este mismo estado se les asigna a ambas silencio directamente. 

El **estado maybe silence** utliza las mismas decisiones que el estado maybevoice con la diferencia que si hay más de dos tramas seguidas en este mismo estado se les asigna a ambas voz directamente.



