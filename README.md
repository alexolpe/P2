PAV - P2: detección de actividad vocal 
============================================
1.Introducción
----------------------------------------------
El objetivo de esta práctica es detectar fragmentos de voz y de silencio en tramas grabadas a lo largo de los últimos cursos por los alumnos de PAV. Para ello se ha implementado un algoritmo en C en el que se han ido realizando diferentes pruebas y experimentos para obtener un resultado con un porcentage de acierto lo mayor posible. Para evaluar la eficiencia del sistema, se han utilizado los parametros *Recall*, *Precision* y *F-score*.

Para empezar a familiarizarse con las señales con las que se trabajan, se ha seguido las órdenes de la primera tarea del enunciado. Para ello, se ha recuperado el fichero .wav que se grabó la clase pasada. Se ha abierto con Wavesurfer y se han etiquetado los segmentos se silencio (S) y voz (V):

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

Una vez familiarizados con las señales con las que vamos a trabajar, proseguimos con la programación del algoritmo.

2.Implementación del detector de voz (VAD).
----------------------------------------------
Para implementar el detector de voz se ha aplicado un sistema de estados. En este, diferenciamos 4 estados distintos:
* ST_INIT: Estado inicial
* ST_VOICE: Voz
* ST_SILENCE: Silencio
* ST_UNDEF_MV: Probablemente voz
* ST_UNDEF_MS: Probablemente silencio

Es importante destacar que en **ST_INIT** es donde se calculan los umbrales de decisión. Para calcular estos umbrales, se utilizan un número determinado por la variable **refsil** de tramas. Mediante estas tramas, calculamos la potencia media del silencio. Analizando los resultados, hemos observado que lo mejor es calcular la media aritmética. Por ello, en lugar de trabajar con la potencia en dBs, la pasamos a Watts. En caso contrario, trabajaríamos con la media geométrica. Una vez calculada dicha potencia media, determinamos dos umbrales distintos sumandole **alpha1** y **alpha2**. Con el objetivo de obtener un mejor resultado, también se decidió implementar un umbral determinado por los cruces por cero. Este es útil para discernir silencio de señales fricativas. Por lo tanto, el criterio de decisión se basa en tres umbrales llamados p1, p2 y zcr.

![image](https://user-images.githubusercontent.com/91085077/159161027-6de331b4-f099-47a6-92e4-25c905a223c2.png)

Con el objetivo de encontrar unos valores óptimos para **refsil**, **alpha1** y **alpha2** se ha implementado lo comentado en el Anexo II de la memoria: pasar dichos parametros por la linea de comandos. Para hacerlo, se ha hecho lo siguiente:

**FALTEN CAPTURES**

Ello, nos ha permitido implementar for's en el **bash** para buscar la combinación de parámetros que daban lugar a un mejor F-score total.

**CAPTURA FOR**
for alpha1 in $(seq 0 1 10 | tr , .); do for alpha2 in $(seq 0 1 10 | tr , .); do for refsil in $(seq 0 1 10 | tr , .); do echo -n  "$alpha1 " "$alpha2" "refsil";scripts/run_vad.sh $alpha1 $alpha2 $refsil | fgrep TOTAL; done done done| sort -t: -k 2n;

Es recomendable buscar cuales son los parámetros que más se ajustan cada vez que se cambie de base de datos.

En cuanto el cambio de estado, el funcionamiento es el siguiente:

**REALMENT CAL FER-HO AIXÍ???????????**

El **ST_SILENCE** decide que si una trama supera el umbral p1 pasa al estado maybe voice y si no lo supera se queda en silencio.

El **ST_VOICE** decide que si una trama no supera el umbral p2 y las cruces por zero son mayores que la media de silencio se pasa al estado maybe silence.

El **ST_UNDEF_MV** decide que si la potencia de una trama es menor que la del umbral p1 el estado pasa a silencio, si la potencia de la trama es mayor que el umbral p2 pasa a voice y si hay más de 2 tramas seguidas en este mismo estado se les asigna a ambas silencio directamente **<-MALAMENT**. 

El **ST_UNDEF_MS** utliza las mismas decisiones que el estado maybevoice con la diferencia que si hay más de dos tramas seguidas en este mismo estado se les asigna a ambas voz directamente.


3.Utilización del VAD
----------------------------------------------

Para obtener los mejores resultados primero se deben obtener los parámetros alpha1, alpha2 y refsil óptimos. Para eso utlizamos el comando for descrito anteriormente. La mejor combinación de estos tres parámetros ha sido alpha1 = 9, alpha2 = 3 y refsil = 5. Si se corre el progrmaa scripts/run_vad.sh 9 3 5 obtenemos el siguiente resultado:

![Captura5](https://user-images.githubusercontent.com/91128741/159157405-fa0df55b-b01d-48e0-9bda-25e006d7a71a.JPG)

Vemos que es un porcentaje de acierto bastante elevado ya que utlizamos el detector con una base de datos muy grande y los parámetros utlizados no serán los mejores para cada caso en particular. Hemos diseñado un detector autómata que encuentra los parámetros que mejor se adaptan para tratar esta base de datos.

Se puede ver el percentaje de acierto que tiene nuestro detector cuando comparamos nuestro audio.

![unknown](https://user-images.githubusercontent.com/91128741/159158074-c1c437b8-c3d7-44e1-96bd-ac3699c852f2.png)

Este resultado no es el mejor que podemos obtener para este caso pero nos interesa la combinación de parámetros que mejoren el F-Score de toda la base de datos no de un solo fichero.



