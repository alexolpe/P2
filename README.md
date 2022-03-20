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

Con el objetivo de encontrar unos valores óptimos para **refsil**, **alpha1** , **alpha2** y **framer** (que representa el FRAME_TIME declarado como constante en el código original) se ha implementado lo comentado en el Anexo II de la memoria: pasar dichos parametros por la linea de comandos. Para hacerlo, se ha hecho lo siguiente:

Primeramente, se ha modificado el fichero vad.docopt de la siguiente manera:

![image](https://user-images.githubusercontent.com/91085077/159172751-05b9076d-461a-4c33-94a8-3f474de14229.png)

Al ejecutar el comando *docopt_c/docopt_c.py src/vad.docopt -o src/vad_docopt.h* se genera el fichero vad_docopt.h

Posteriormente, se modifica el fichero main_vad.c para operar con los parametros introducidos por la linea de comandos:

![image](https://user-images.githubusercontent.com/91085077/159173075-dd354fec-3869-40e5-9b45-e5cfa1f725c0.png)

Finalmente, también se adapta el fichero run_vad.sh para poder clasificar voz y silencio en todas las señales de la base de datos con la instrucción *scripts/run_vad.sh alpha1 alpha2 refsil framer* sustituyendo las últimas 4 variables por su respectivo valor.

![image](https://user-images.githubusercontent.com/91085077/159173221-6283c8df-842b-4174-a816-4c599c1d62f3.png)

Otra utilidad ha sido que ha permitido implementar for's en el **bash** para buscar la combinación de parámetros que daban lugar a un mejor F-score total.

*for alpha1 in $(seq 0 1 10 | tr , .); do for alpha2 in $(seq 0 1 10 | tr , .); do for refsil in $(seq 0 1 10 | tr , .); do echo -n  "$alpha1 " "$alpha2" "refsil";scripts/run_vad.sh $alpha1 $alpha2 $refsil 20 | fgrep TOTAL; done done done| sort -t: -k 2n;*

Destacar que a **framer** (FRAME_TIME) se le ha dado el valor de 20ms ya que se ha observado que es el valor que da mejores resultados

Es recomendable buscar cuales son los parámetros que más se ajustan cada vez que se cambie de base de datos.

En cuanto el cambio de estado, el funcionamiento es el siguiente:

* **ST_SILENCE**: si una trama supera el umbral p1, se pasa al estado ST_UNDEF_MV.

![image](https://user-images.githubusercontent.com/91085077/159173520-23c6f946-c253-4fe0-b193-f30bf1b46a0e.png)

* **ST_VOICE**: si una trama no supera el umbral p2 y las cruces por zero son inferiores al umbral zcr, se pasa al estado ST_UNDEF_MS.

![image](https://user-images.githubusercontent.com/91085077/159173537-0aefcc5b-4bc3-4ac1-b946-43ff28dd23a3.png)

* **ST_UNDEF_MV**: si la potencia de una trama es menor que la del umbral p1, el estado pasa a ST_SILENCE. si la potencia de la trama es mayor que el umbral p2, pasa a ST_VOICE. Si ya lleva 2 tramas clasificadas en el estado ST_UNDEF_MV, vuelve al estado ST_SILENCE. 

![image](https://user-images.githubusercontent.com/91085077/159173572-e9213c20-6ec0-48f9-b19c-7498710a9bfb.png)

* **ST_UNDEF_MS** utliza las mismas decisiones que el estado ST_UNDEF_MV con la diferencia que si ya lleva 2 tramas clasificadas en el estado ST_UNDEF_MS, vuelve al estado ST_VOICE. 

![image](https://user-images.githubusercontent.com/91085077/159173595-109c42e2-172f-4fb9-9324-50e5f312e96c.png)


3.Utilización del VAD
----------------------------------------------
Primero de todo, para obtener los mejores resultados, se deben obtener los parámetros alpha1, alpha2 y refsil óptimos. Para eso utlizamos el for descrito anteriormente. Los resultados obtenidos nos indican que, para la base de datos con la que estamos trabajando, la mejor combinación es alpha1 = 9, alpha2 = 3 y refsil = 5.

Como se ha mencionado anteriormente, en el VAD se trabaja con 4 estados. Es por ello que en un inicio, al ejecutar el programa, se etiquetan los fragmentos de la señal de la siguiente manera:

![Captura](https://user-images.githubusercontent.com/91085077/159166984-2d7ebbe3-2d62-45e9-8de7-674a4b38e30f.PNG)

Sin embargo, las señales sólo debe tener las etiquetas de voy o silencio. Para ello, se ha implementado el siguiente código en el fichero main_vad.c dentro del for que va recorriendo la señal por tramas:

![image](https://user-images.githubusercontent.com/91085077/159167094-d2bad48f-676e-440e-8964-3f692a3c939b.png)

Ahora, si se vuelve a ejecutar el programa, el resultado es el siguiente:

![image](https://user-images.githubusercontent.com/91085077/159167171-0987d04d-4bba-4ca9-89fe-1451e6bd2c87.png)


Una vez el etiquetado es el que se desea, si se ejecuta scripts/run_vad.sh 9 3 5 (donde 9, 3 y 5 hacen referencia a alpha1, alpha2 y refsil respectivamente) obtenemos el siguiente resultado:

![Captura5](https://user-images.githubusercontent.com/91128741/159157405-fa0df55b-b01d-48e0-9bda-25e006d7a71a.JPG)

Se observa que es un porcentaje de acierto bastante elevado ya que utlizamos el detector con una base de datos muy grande y los parámetros utlizados no serán los mejores para cada caso en particular. Hemos diseñado un detector autómata que encuentra los parámetros que mejor se adaptan para tratar esta base de datos.

Se puede ver el percentaje de acierto que tiene el detector cuando se quieren clasificar los fragmentos de voz y silencio del audio que se grabó en la práctica anterior.

![unknown](https://user-images.githubusercontent.com/91128741/159158074-c1c437b8-c3d7-44e1-96bd-ac3699c852f2.png)


4.Sustitución del silencio por ceros
----------------------------------------------
De acuerdo con la propuesta realizada por el ejercicio de ampliación del enunciado, se han añadido código en el fichero main_vad.c con el objetivo de sustituir los fragmentos considerados silencio por ceros:

![image](https://user-images.githubusercontent.com/91085077/159171935-b83095bd-26d9-41b0-be2a-e0eeb1185d81.png)

![image](https://user-images.githubusercontent.com/91085077/159171966-824f6eae-d7cb-4a70-92a0-1acc0c97c750.png)

Para evaluar el resultado, se compara la señal original:

![image](https://user-images.githubusercontent.com/91085077/159172093-a5e92e5d-622b-4483-82e3-5b35cb56cff6.png)

Con la señal resultante:

![image](https://user-images.githubusercontent.com/91085077/159172146-8738ac0b-13be-4cfc-a5f8-5b4daf0770fc.png)


5.Conclusiones
----------------------------------------------
En esta práctica se ha aprendido y trabajado con un algoritmo que permite clasificar fragmentos de una señal en voz y silencio. Esta es una utilidad muy práctica ya que nombrosas aplicaciones tecnológicas la utilizan. Por ejemplo, aplicaciones para hacer videoconferencias, como Meet o Zoom, cuando detectan que una persona con el micro abierto no está hablando, lo silencian.

La realización de esta práctica ha sido distinta a lo que se está acostumbrado. Hasta el momento, los trabajos de programación habían consistido en escribir código para que, al ejecutar, apareciera algo por pantalla. El objetivo se limitava a obtener lo que se pedía y a tener un código que respetara el orden y normas de la programación. Sin embargo, en este trabajo el objetivo iba más allá ya que no tan sólo teníamos que lograr etiquetar una señal con fragmentos de voz y silencio, sino que teníamos que encontrar la forma más eficiente para hacerlo. Para ello, hemos aprendido a trabajar con parámetros que evaluan la eficiencia de nuestro sistema.

También nos hemos familiarizado con un estilo de programación distinto basado en ir probando distintas estrategias para obtener el mejor resultado. Durante nuestras pruebas, el algoritmo que mejor ha funcionado es el que se ha mostrado. Sin embargo, hemos probado distintas estrategias como trabajar con 2 estados únicamente o modificar las condiciones de los cambios de estado que no han dado los resultados esperados.

