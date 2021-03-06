/* echo_client.c: Message Queue Echo Client test */

#include "mq/client.h"
#include "mq/string.h"
#include <assert.h>
#include <time.h>
#include <unistd.h>

/* Constants */

const char * TOPIC     = "chatroom";

/* Threads */

void *incoming_thread(void *arg) {
    MessageQueue *mq = (MessageQueue *)arg;

    while (!mq_shutdown(mq)) {
    	char *message = mq_retrieve(mq);
	if (message) {
            printf("%s\n", message);
	    free(message);
	}
    }
    
    return NULL;
}

void *outgoing_thread(void *arg) {
    MessageQueue *mq = (MessageQueue *)arg;
    
    char body[BUFSIZ];
    char on_enter[BUFSIZ];
    
    sprintf(on_enter, "%s has entered the chatroom!\n", mq->name);
    mq_publish(mq, TOPIC, "============================");
    mq_publish(mq, TOPIC, on_enter);
    mq_publish(mq, TOPIC, "============================");

    
    while (fgets(body, BUFSIZ, stdin) && !streq(body, "/exit\n")) {
        char buffer[BUFSIZ];

        strcpy(buffer, mq->name);
        strcat(buffer, ": ");
        strcat(buffer, body);

        mq_publish(mq, TOPIC, buffer);
    }   
    
    sprintf(on_enter, "%s has left the chatroom!\n", mq->name);

    mq_publish(mq, TOPIC, on_enter);
    mq_stop(mq);

    return NULL;
}

/* Main execution */

int main(int argc, char *argv[]) {
    /* Parse command-line arguments */
    char *name = getenv("USER");
    char *host = "localhost";
    char *port = "9620";
    
    if (argc > 1) { host = argv[1]; }
    if (argc > 2) { port = argv[2]; }
    if (argc > 3) { name = argv[3]; }

    /* Create and start message queue */
    MessageQueue *mq = mq_create(name, host, port);
    

    mq_subscribe(mq, TOPIC);
    mq_start(mq);

    /* Run and wait for incoming and outgoing threads */
    Thread incoming;
    Thread outgoing;
    thread_create(&incoming, NULL, incoming_thread, mq);
    thread_create(&outgoing, NULL, outgoing_thread, mq);
    thread_join(incoming, NULL);
    thread_join(outgoing, NULL);

    mq_delete(mq);
    return 0;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */ 
