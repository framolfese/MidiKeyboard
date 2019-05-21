**Progetto SO** 

Francesco Molfese 1760914

Francesco Douglas Scotti Di Vigoleno 1743635

**What** 

Il progetto consiste nell'implementazione di una tastiera midi tramite arduino mega 2560.
La board in questione ha quattro tasti collegati che corrispondono rispettivamente alle note: Do, Re, Mi e Fa. I loro valori verranno inviati sulla seriale e convertiti tramite la loro frequenza in PCM (Pulse Code Modulation) e successivamente riprodotti.

**How**

*Sezione relativa alla board* 

Viene dichiarata una struct di nome Tone contenente le variabili da gestire di ogni nota, ovvero il suo valore di riconoscimento e un campo variabile di nome "on" che indica se la nota è stata premuta o rilasciata.
Si lavora sulla porta F della board con i pin 2,3,4 e 5. Ogni pin è dedicato ad una specifica nota.
Successivamente sulla seriale verranno mandati i ordine i valori di sincronizzazione per la seriale, ovvero: 0x55, 0xaa,  il buffer contenente la struct, il checksum e infine nuovamente 0xaa (tutto con un minimo delay tra un invio e l'altro).

*Sezione relativa al programma principale*

Viene utilizzata la libreria di OpenAL per la gestione del suono.
Si dichiarano dei buffer che verranno gestiti dai rispettivi threads di appartenenza che serviranno per: l'inizializzazione del device con cui comunicare,  il contesto di output su cui lavorare, il buffer dove scrivere il pcm, il buffer di riproduzione del pcm e infine il buffer contenente lo stato corrente.
Una volta aperta la seriale si procederà all'inizio del protocollo di comunicazione con la seriale diviso in tante sotto-funzioni le quali controlleranno l'autenticità di ogni singolo dato ricevuto dalla seriale nonché il loro ordine di ricezione.
Arrivati alla fine della ricezione dei dati, si controlla che il Checksum ricevuto sia lo stesso ottenuto dalla riapplicazione del Checksum sul buffer ottenuto, in caso di esito positivo si procede, altrimenti si scarta il tutto e si aspetta la ricezione del prossimo dato.
Se l'operazione è andata a buon fine si procede con l'identificazione della nota ricevuta assegnandole un valore rappresentante la sua frequenza e un indice indicante quale thread i-esimo dovrà eseguire quella nota.
Prima di creare il thread e fargli eseguire la funzione che riprodurrà il suono, si controlla se il campo "on" della nota è già settato a 1, in caso positivo si manda un messaggio di errore perché significa che c'è stato un errore nella ricezione dei valori in quanto una nota che sta già suonando non può ovviamente essere suonata nuovamente.
In caso il campo è settato a 0, si setta il campo a 1 per indicare che la nota sta per essere suonata e si fa partire il thread i-esimo che la gestirà. 
La funzione per riprodurre il suono ha il funzionamento seguente:

- Si prende il valore dell'indice i-esimo associato ai buffer di cui sopra e il valore della frequenza della nota da suonare
- Si inizializzano le strutture OpenAL associate al thread i-esimo
- Si crea un Campione che conterrà un'onda sinusoidale rappresentante il pcm con rispettivo rate, frequenza, ampiezza e un valore che cambierà nel tempo per far si che la nota svanirà nel tempo (come nel pianoforte quando si preme una nota e non si tiene premuto il pedale del sustain).
- Il Campione inizializzato viene caricato nel rispettivo buffer interno per la gestione del suono e successivamente riprodotto finché non si riceve la stessa nota ma con campo "on" diverso dal valore attuale, che significa che bisogna smettere di suonare la nota perché è stata rilasciata.
- Una volta rilasciata la nota si procede alla free del Campione usato e alla chiusura delle strutture usate per la gestione di OpenAL
- Infine una volta chiuso tutto, si attende la ricezione della prossima nota (essendo tutto gestito con i threads, è possibile premere più note insieme essendo i processi gestiti separatamente e in modo simultaneo).

**How to run**

Nella cartella CodiePC è presente un eseguibile generato dal Makefile, basta eseguirlo e attendere la ricezione di una nota.
Una volta avviato il Main sopracitato, sarà possibile iniziare a premere i quattro tasti presenti sulla board (da Do a Fa rispettivamente partendo da sinistra) attendendo qualche ms per la sincronizzazione iniziale.
Ogni volta che viene premuta una nota viene printato a video il nome della nota, il suo valore e il campo "on".
Per terminare il programma, premere semplicemente Ctrl-C (ovviamente verrà gestita l'azione per quest'ultimo comando). 