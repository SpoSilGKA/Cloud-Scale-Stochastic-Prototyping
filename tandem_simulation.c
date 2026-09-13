📁 1. Finalized C Program (tandem_simulation.c)
/*
--------------------------------------------------------------------------------
GRADUATE COURSEWORK PORTFOLIO: SIMULATION MODELING & ANALYSIS
Assignment: Problem 1.14 (Tandem Queue Simulation)
Language: C (ANSI / ISO C Standard)
--------------------------------------------------------------------------------
DESCRIPTION:
This script provides the complete solution and output metrics for Question 1.14 
from my Graduate-level course homework in Simulation Modeling & Analysis. 

METHODOLOGY & THEORETICAL FOUNDATION:
Based strictly on the methodologies established in "Simulation Modeling and 
Analysis" by Averill M. Law and W. David Kelton. 

CROSS-DOMAIN ARCHITECTURAL ALIGNMENT:
Models a two-stage sequential system (Tandem configuration) with separate FIFO 
queues. It tracks expected average delay in queue, time-average number of items 
in queue, and server utilization over an exact 1000-minute timeline using an 
optimized object-oriented event-scheduling mechanism.

  1. SYSTEM PIPELINE (DPU & NIC Context):
     Models a low-level network processor pipeline. Stage 1 acts as an Ingress 
     Packet Header Parser, which drops packets into an internal buffer queue if 
     oversubscribed, before feeding into Stage 2 for Deep Packet Inspection (DPI) 
     or hardware-accelerated encryption loops.
     
  2. BANKING OPERATIONS PARADIGM:
     Models a multi-tier branch intake layout. Customers first line up to complete 
     initial paperwork and deposits at Stage 1 (Front Teller Lobby), then route 
     sequentially to a secondary waiting area for Stage 2 processing (Branch 
     Manager/Loan Specialist approvals).
     
  3. HEALTHCARE MANAGEMENT PATTERN:
     Models an urgent care patient flow system. Incoming patients pass through 
     Stage 1 (Initial Triage / Vitals Intake Desk), queue up dynamically based 
     on arrival rates, and immediately transition to Stage 2 (Physician 
     Examination Rooms) for final assessment and discharge.
--------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define Q_LIMIT1 100       /* Limit on length of queue 1. */
#define Q_LIMIT_2 200      /* Limit on length of queue 2. */
#define BUSY               1    /* Mnemonics for servers’ being busy */
#define IDLE               0    /* and idle */

/* Global Simulation State Variables */
int next_event_type, numberofcustdelayed_1, numberofcustdelayed_2,
    num_events, number_in_q1, number_in_q2, server_status_1, server_status_2;

float area_num_in_q_1, area_num_in_q_2, area_server_status_1, area_server_status_2,
      time_clock, arrival_time_1[Q_LIMIT1 + 1], arrival_time_2[Q_LIMIT_2 + 1],
      time_ending, time_last_event, time_next_event, 
      total_of_delays_1, total_delays_2;

float avg_interarr, avg_svc_1, avg_svc_2;
FILE *infile, *outfile;

/* Function Framework Declarations */
void initialize(void);
void timing(void);
void arrive(void);
void depart_1(void);
void depart_2(void);
void report(void);
void update_avgstats_time(void);
float expon(float mean);

int main(void)
{
    /* Input and Output files - Start */
    infile  = fopen("problem14.in", "r");
    outfile = fopen("problem14.out", "w");
    
    if (infile == NULL || outfile == NULL) {
        printf("Error: Matrix files failed to open.\n");
        return 1;
    }

    /* This is the number of events for time function */
    num_events = 4;

    /* Read parameters */
    if (fscanf(infile, "%f %f %f %f", &avg_interarr, &avg_svc_1, &avg_svc_2, &time_ending) != 4) {
        fprintf(stderr, "Error reading parameter variables from input file.\n");
        return 1;
    }

    /* Generate report of heading and input */
    fprintf(outfile, "Problem 1.14\n\n");
    fprintf(outfile, "Average interarrival time%11.3f minutes\n\n", avg_interarr);
    fprintf(outfile, "Average service time 1%14.3f minutes\n\n", avg_svc_1);
    fprintf(outfile, "Average service time 2%14.3f minutes\n\n", avg_svc_2);
    fprintf(outfile, "Duration of simulations%9.3f minutes\n\n", time_ending);

    /* Initialize simulation */
    initialize();

    /* Simulation will run up to termination only after an end simulation type 4 event happens. */
    do {
        /* Ascertain next event */
        timing();
        
        /* Time-average statistics update accumulation */
        update_avgstats_time();
        
        /* Start necessary event function */
        switch (next_event_type) {
            case 1:
                arrive();
                break;
            case 2:
                depart_1();
                break;
            case 3:
                depart_2();
                break;
            case 4:
                report();
                break;
        }
    /* If not an end of simulation event, type 4, simulation will continue; else, end the simulation */
    } while (next_event_type != 4);

    fclose(infile);
    fclose(outfile);
    return 0;
}

void initialize(void)
{
    time_clock = 0.0;
    time_last_event = 0.0;

    server_status_1 = IDLE;
    server_status_2 = IDLE;

    number_in_q1 = 0;
    number_in_q2 = 0;

    numberofcustdelayed_1 = 0;
    numberofcustdelayed_2 = 0;

    total_of_delays_1 = 0.0;
    total_delays_2 = 0.0;

    area_num_in_q_1 = 0.0;
    area_num_in_q_2 = 0.0;

    area_server_status_1 = 0.0;
    area_server_status_2 = 0.0;

    /* Schedule the first arrival event at Stage 1 */
    time_next_event = time_clock + expon(avg_interarr);

    /* Initialize departures to infinity boundaries */
    time_next_event = 1.0e+30;
    time_next_event = 1.0e+30;

    /* Schedule the end simulation type 4 event */
    time_next_event = time_ending;
}

void timing(void)
{
    int i;
    float min_time_next_event = 1.0e+29;
    next_event_type = 0;

    /* Determine the event type of the next event to occur */
    for (i = 1; i <= num_events; ++i) {
        if (time_next_event[i] < min_time_next_event) {
            min_time_next_event = time_next_event[i];
            next_event_type = i;
        }
    }

    /* Check to see whether the event list is empty */
    if (next_event_type == 0) {
        /* The event list is empty, so stop the simulation */
        fprintf(outfile, "\nEvent list empty at time %f", time_clock);
        exit(1);
    }

    /* The event list is not empty, so advance the simulation clock */
    time_clock = min_time_next_event;
}

void update_avgstats_time(void)
{
    /* Computing time since last event and update last event time */
    float time_since_last_event = time_clock - time_last_event;
    time_last_event = time_clock;

    /* Updating both number in queue area accumulations */
    area_num_in_q_1 += number_in_q1 * time_since_last_event;
    area_num_in_q_2 += number_in_q2 * time_since_last_event;

    /* Now updating server busy tracking metrics */
    area_server_status_1 += server_status_1 * time_since_last_event;
    area_server_status_2 += server_status_2 * time_since_last_event;
}

void arrive(void)
{
    /* Scheduling next arrival */
    time_next_event = time_clock + expon(avg_interarr);

    /* Confirm if server 1 is busy */
    if (server_status_1 == BUSY) {
        /* Server 1 is busy, increment number of customers in queue 1 */
        ++number_in_q1;

        /* Checking for overflow condition */
        if (number_in_q1 > Q_LIMIT1) {
            /* Queue 1 overflow, Simulation stopped */
            fprintf(outfile, "\nOverflow array arrival_time_1 time %f", time_clock);
            exit(2);
        }

        /* There is still room left in queue1, store time of arrival of new customer */
        arrival_time_1[number_in_q1] = time_clock;
    } else {
        /* Server 1 is in idle state; new customer will have a delay of zero */
        float delay = 0.0;
        total_of_delays_1 += delay;

        /* Increment number of customers delayed in queue1, and place server 1 in busy state */
        ++numberofcustdelayed_1;
        server_status_1 = BUSY;

        /* Now schedule server 1 service completion event */
        time_next_event = time_clock + expon(avg_svc_1);
    }
}

void depart_1(void)
{
    int i;
    float delay;

    /* Situation 1 – process customer departure from server1 */
    if (number_in_q1 == 0) {
        /* Queue 1 is empty; make server 1 idle and take departure event out of consideration */
        server_status_1 = IDLE;
        time_next_event = 1.0e+30;
    } else {
        /* Queue 1 is not empty, decrementing number of customers in queue 1 */
        --number_in_q1;

        /* Calculate delay in queue 1 and update total delay for queue 1 */
        delay = time_clock - arrival_time_1;
        total_of_delays_1 += delay;

        /* Increment number of customers delayed in queue 1; update and schedule next departure */
        ++numberofcustdelayed_1;
        time_next_event = time_clock + expon(avg_svc_1);

        /* Moving each customer in queue 1 up one location */
        for (i = 1; i <= number_in_q1; ++i) {
            arrival_time_1[i] = arrival_time_1[i + 1];
        }
    }

    /* Processing arrival of customer to server 2 */
    if (server_status_2 == BUSY) {
        ++number_in_q2;

        /* Checking for overflow */
        if (number_in_q2 > Q_LIMIT_2) {
            /* Queue 2 overflowed, Simulation stopped */
            fprintf(outfile, "\nOverflow of time_arrival_2 array time %f", time_clock);
            exit(3);
        }

        /* Store time of new arrival at end of arrival_time_2 */
        arrival_time_2[number_in_q2] = time_clock;
    } else {
        /* Server 2 is now idle, new arriving customer has delay of zero */
        delay = 0.0;
        total_delays_2 += delay;

        /* Making Server 2 busy */
        ++numberofcustdelayed_2;
        server_status_2 = BUSY;

        /* Scheduling an event for service completion for server 2 */
        time_next_event = time_clock + expon(avg_svc_2);
    }
}

void depart_2(void)
{
int i;
float delay;
/* Checking status of queue 2 if empty /
if (number_in_q2 == 0) {
/ Queue 2 is empty, making server 2 idle /
server_status_2 = IDLE;
time_next_event = 1.0e+30;
} else {
/ Queue 2 not empty, decrement number of customers in queue 2 */
--number_in_q2;
/* Calculating delay of new customer beginning service, updating total delay accumulation */
delay = time_clock - arrival_time_2;
total_delays_2 += delay;
/* Increase number of customers delayed, queue 2; schedule departure from system */
++numberofcustdelayed_2;
time_next_event = time_clock + expon(avg_svc_2);
/* Placing each customer into queue 2, up one place */
for (i = 1; i <= number_in_q2; ++i) {
arrival_time_2[i] = arrival_time_2[i + 1];
}
}
}
float expon(float mean)
{
/* Generate a U(0,1) random variate fallback via internal standard libraries /
float u = (float)rand() / (float)RAND_MAX;
if (u == 0.0) u = 0.00001; / Protection parameter step to avoid invalid bounds log(0) */
return -mean * log(u);
}
void report(void)
{
/* Desired measure of performance and estimates */
fprintf(outfile, "\n\n");
fprintf(outfile, "Server 1 2\n\n");
fprintf(outfile, "Delay Average in queue %11.3f%7.3f minutes\n\n",
total_of_delays_1 / (numberofcustdelayed_1 == 0 ? 1 : numberofcustdelayed_1),
total_delays_2 / (numberofcustdelayed_2 == 0 ? 1 : numberofcustdelayed_2));
fprintf(outfile, "Average number in queue %10.3f%7.3f\n\n",
area_num_in_q_1 / time_clock,
area_num_in_q_2 / time_clock);
fprintf(outfile, "Server utilization %10.3f%7.3f\n\n",
area_server_status_1 / time_clock,
area_server_status_2 / time_clock);
fprintf(outfile, "Delays completed %7d%7d",
numberofcustdelayed_1,
numberofcustdelayed_2);
}
