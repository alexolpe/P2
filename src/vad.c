// librerias publicas
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// librerias privadas, nuestras
#include "pav_analysis.h"
#include "vad.h"

const float FRAME_TIME = 20.0F; /* in ms. */

/*
 * As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
 * only this labels are needed. You need to add all labels, in case
 * you want to print the internal state in string format
 */

const char *state_str[] = {
    "UNDEF", "S", "V", "INIT"};

const char *state2str(VAD_STATE st)
{
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct
{
  float zcr;
  float p;
  float am;

} Features;

/*
 * TODO: Delete and use your own features!
 */

Features compute_features(const float *x, int N)
{
  /*
   * Input: x[i] : i=0 .... N-1
   * Ouput: computed features
   */
  /*
   * DELETE and include a call to your own functions
   *
   * For the moment, compute random value between 0 and 1
   */
  Features feat;
  // feat.zcr = feat.p = feat.am = (float) rand()/RAND_MAX;
  feat.zcr = compute_zcr(x, N, 16000);
  feat.am = compute_am(x, N);
  feat.p = compute_power(x, N);

  return feat;
}

/*
 * TODO: Init the values of vad_data
 */

VAD_DATA *vad_open(float rate, float alpha1, float alpha2, float refsil)
{
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  vad_data->alpha1 = alpha1;
  vad_data->alpha2 = alpha2;
  vad_data->refsil = refsil;
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data)
{
  /*
   * TODO: decide what to do with the last undecided frames
   */
  VAD_STATE state = vad_data->last_state;
  // VAD_STATE state=vad_data->state;
  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data)
{
  return vad_data->frame_length;
}

/*
 * TODO: Implement the Voice Activity Detection
 * using a Finite State Automata
 */

VAD_STATE vad(VAD_DATA *vad_data, float *x, unsigned int t)
{

  /*
   * TODO: You can change this, using your own features,
   * program finite state automaton, define conditions, etc.
   */

  Features f = compute_features(x, vad_data->frame_length);
  vad_data->last_feature = f.p; /* save feature, in case you want to show */
                                /*f.p=power feature*/

  switch (vad_data->state)
  {

  case ST_INIT:
    if (t < vad_data->refsil)
    {
      vad_data->watts += pow(10, f.p / 10);
      vad_data->zcr += f.zcr;
      // printf("%f\n", vad_data->p1);
    }
    else
    {
      vad_data->p1 = 10 * log10(vad_data->watts / t) + vad_data->alpha1;
      vad_data->p2 = vad_data->p1 + vad_data->alpha2; // alpha2
      vad_data->zcr = vad_data->zcr / t;
      vad_data->state = ST_SILENCE;
    }
    break;

  case ST_SILENCE:

    vad_data->num_UNDEF = 0;
    if (f.p > vad_data->p1)
    {

      vad_data->state = ST_UNDEF_MV;
    }
    else if (f.p > vad_data->p2)
    {
      vad_data->state = ST_VOICE;
    }

    break;

  case ST_VOICE:

    vad_data->num_UNDEF = 0;

    if (f.p < vad_data->p2 && vad_data->zcr > f.zcr)
    {
      vad_data->state = ST_UNDEF_MS;
    }
    else if (f.p < vad_data->p1 && vad_data->zcr > f.zcr)
    {
      vad_data->state = ST_SILENCE;
    }
    break;

  case ST_UNDEF_MS:

    vad_data->num_UNDEF += 1;
    if (f.p < vad_data->p1)
    {
      vad_data->state = ST_SILENCE;
    }
    else if (f.p > vad_data->p2)
    {
      vad_data->state = ST_VOICE;
    }
    else if (vad_data->num_UNDEF > 1)
    {
      vad_data->state = ST_VOICE;
    }

    break;
  case ST_UNDEF_MV:

    vad_data->num_UNDEF += 1;
    if (f.p < vad_data->p1)
    {
      vad_data->state = ST_SILENCE;
    }
    else if (f.p > vad_data->p2)
    {
      vad_data->state = ST_VOICE;
    }
    else if (vad_data->num_UNDEF > 1)
    {
      vad_data->state = ST_SILENCE;
    }

    break;

  case ST_UNDEF:

    break;
  }

  if (vad_data->state == ST_SILENCE ||
      vad_data->state == ST_VOICE ||
      vad_data->state == ST_UNDEF_MS ||
      vad_data->state == ST_UNDEF_MV)
    return vad_data->state;

  else if (vad_data->state == ST_INIT)
    return ST_SILENCE;
}

void vad_show_state(const VAD_DATA *vad_data, FILE *out)
{
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
