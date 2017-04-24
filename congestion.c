//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************



int cwnd=0, ssthresh=64000, slowStartCounter=0, congestionAvoidanceCounter=0; //
int MSS = (int)sizeof(purData); //+header mais on a pas de header

//Variables for analysis
int max_congestion_window=0, max_packets_sent=0, packet_counter=0;;


//Function to implement congestion control. Indicates a timeout of parameter isTimeout is true.

//J'AI CODÉ LA FONCTION AVEC LES EXPLICATIONS DE LA MISE A JOUR DE LA FENETRE DE CONGESTION
//MAIS JE PENSE PAS QU'IL FAILLE METTRE UN TRUC POUR DOUBLE LA TAILLE DE LA FENETRE SI ON
//APPELLE CETTE FONCTION À CHAQUE ENVOIE D'UNE TRAME AU FINAL CA FERA FOIS DEUX SA TAILLE LA FIN

void updateCongestionWindow(bool isTimeOut)
{
  if(isTimeOut)
  {
    printf("Now it's slow start mode ... \n");
    ssthresh = cwnd/2;
    cwnd = MSS;
    slowStartCounter++;
  }
  else
  {
    //Au-delà d'une certaine limite de valeur de cwnd (slow start threshold, ssthresh)
    //TCP passe en mode d'évitement de congestion.
    //À partir de là, la valeur de cwnd augmente de façon linéaire et donc bien plus
    //lentement qu'en slow start : cwnd s'incrémente de un MSS (= un paquet) à chaque RTT
    //Dans ce mode de fonctionnement, l'algorithme détecte aussi rapidement que possible
    //la perte d'un paquet : si nous recevons trois fois le ACK même paquet, on n'attend
    //pas la fin d'un timeout pour réagir. En réaction à cette perte, on fait descendre
    //La valeur de ssthresh ainsi que cwnd (on repasse éventuellement en mode de Slow Start).
    //On utilise la technique de Fast Retransmit pour renvoyer rapidement les paquets perdus.
    if(cwnd<ssthresh)
    {
      cwnd = cwnd + MSS;
      slowStartCounter++;
    }
    else
    {
      printf("Now it's congestion avoidance mode...\n");
      cwnd = cwnd + MSS * (MSS/cwnd);
      congestionAvoidanceCounter++;
    }
  }
}
//Plutôt que repasser en mode Slow Start lors d'une duplication de ACK (et après un passage
//par le mode Fast Retransmit), nous renvoyons le segment perdu et on attend un ACK pour
//toute la fenêtre transmise précédemment avant de retourner en mode Congestion Avoidance.
//Si on atteint le timeout, on repart en mode Slow Start. Grâce à cette technique nous évitons
//de baisser le débit d'une façon trop brutale.



//Calculates the timeout using Jacobson/Kragel's algorithm.
struct timeval calculateTimeout(long sampleRTT) //sampleRTT valeur courante de notre RTT
{
  struct timeval struct_timeout;

  estimatedRTT = (0.875 * estimatedRTT) + (0.125 * sampleRTT); //estimatedRTT valeur estimé de notre RTT
  devRTT = (0.75 * devRTT) + (0.25 * labs(RTT - estimatedRTT)); //labs c'est la fonction valeur absolue, devRTT coefficient de variation de la RTT

  timeout = est_rtt + 4 * devRTT;
  printf("RTT: %ld\n",timeout);

  if(timeout == 0)  timeout++;

  struct_timeout.tv_sec = timeout/1000000;
  struct_timeout.tv_usec = timeout % 1000000;

  return struct_timeout;

}
