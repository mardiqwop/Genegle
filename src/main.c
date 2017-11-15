// Ai Project
// Genetic ALgorithm for Musical Accompaniment

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "fitness.h"

#include <jack/jack.h>
#include <jack/midiport.h>

#define CLIENT_NAME "Genegle"

// create MIDI ports as global variables
jack_port_t *midi_input_port;
jack_port_t *midi_output_port;
jack_client_t *client;

unsigned char note = 0; // gets updated in the process callback from MIDI messages

void jack_shutdown(void *arg){
    exit(1);
}



// callback function for whenever the soundcard is ready for more output
// This program does not actually output any audio, but this can be good
// in conjunction with knowing the sampling rate to do accurate timing
// nframes is not how many "chunks," it is how many samples
int process(jack_nframes_t nframes, void *arg){

    void* port_buffer = jack_port_get_buffer(midi_input_port, nframes);
    
    jack_midi_event_t input_event;
    jack_nframes_t event_count = jack_midi_get_event_count(port_buffer);
    jack_nframes_t event_index = 0; // for looping over all midi events

    jack_midi_event_get(&input_event, port_buffer, 0);
    for(int i=0; i<nframes; i++)
    {
	if((input_event.time == i) && (event_index < event_count))
	{
	    // mask with 11110000
	    // A note on message is 10010000
	    // The next byte will be the note value
	    if( ((*(input_event.buffer) & 0xf0)) == 0x90 )
	    {
		/* note on */
		note = *(input_event.buffer + 1);
		printf("%d\n", note);
	    }
	    event_index++;
	    if(event_index < event_count)
		jack_midi_event_get(&input_event, port_buffer, event_index);
	}
    }
	
    return 0;
}

// callback for when sampling rate gets changed
int srate(jack_nframes_t nframes, void *arg)
{
    printf("the sample rate is now %" PRIu32 "/sec\n", nframes);
    return 0;
}

int main(void){

    client = jack_client_open(CLIENT_NAME, JackNullOption, NULL);
    if(client == 0){
	printf("\nCould not start as a JACK client. Make sure the jack server is running.\n");
	return 1;
    }
    jack_on_shutdown(client, jack_shutdown, 0);

    midi_input_port = jack_port_register(client, "midi_in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    midi_output_port = jack_port_register(client, "midi_out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);

	jack_set_process_callback (client, process, 0);
	jack_set_sample_rate_callback (client, srate, 0);
    
    if(jack_activate(client) != 0){
	printf("\nERROR: Can't activate the JACK client.\n");
    }

    char command[100];
    int running = 1;
    while(running){
	fgets(command, 99, stdin);
	printf("%s\n", command);
	if(strncmp(command,"exit",4)== 0){
	    running = 0;
	    break;
	}
    }
    jack_client_close(client);
    exit(0);
    
}





void testHist(void){
    Note* n1 = SetupNote(60);
    Note* n2 = SetupNote(62);
    Note* n3 = SetupNote(62);
    Note* t1 = SetupNote(62);
    Note* t2 = SetupNote(62);
    Note* badNote = SetupNote(63);
    
    NoteQueue* qPtr = malloc(sizeof(NoteQueue));
    NoteQueue* otherqPtr = malloc(sizeof(NoteQueue));
    NoteQueue* badqPtr = malloc(sizeof(NoteQueue));
    initNoteQueue(qPtr);
    initNoteQueue(otherqPtr);
    initNoteQueue(badqPtr);
    InsertNote(n1, qPtr);
    InsertNote(n2, qPtr);
    InsertNote(n3, qPtr);
    PrintHistogram(qPtr);
    InsertNote(badNote, badqPtr);
    InsertNote(t1, otherqPtr);
    InsertNote(t2, otherqPtr);
    PrintHistogram(otherqPtr);    
    printf("\n%d", fit(qPtr->histogram, otherqPtr->histogram, badqPtr->histogram));

    free(n1);
    free(n2);
    free(n3);
    free(t1);
    free(t2);
    free(badNote);
    free(qPtr);
    free(otherqPtr);
    free(badqPtr);
}
