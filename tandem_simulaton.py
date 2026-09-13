📁 2. Finalized Python Script (`tandem_simulation.py`)

```python
"""
--------------------------------------------------------------------------------
GRADUATE COURSEWORK PORTFOLIO: SIMULATION MODELING & ANALYSIS
Assignment: Problem 1.14 (Tandem Queue Simulation)
Language: Python 3
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
"""

import math
import random
import sys


class TandemSimulation:
    IDLE = 0
    BUSY = 1

    def __init__(self, q_limit_1=100, q_limit_2=200):
        self.Q_LIMIT1 = q_limit_1
        self.Q_LIMIT_2 = q_limit_2

        # Core State Variables
        self.time_clock = 0.0
        self.time_last_event = 0.0
        self.server_status_1 = self.IDLE
        self.server_status_2 = self.IDLE
        self.number_in_q1 = 0
        self.number_in_q2 = 0

        # Arrays to track timestamps in FIFO arrays
        self.arrival_time_1 = []
        self.arrival_time_2 = []

        # Event List Mapping Matrix (1-indexed matching textbook logic layout)
        self.num_events = 4
        self.time_next_event = [0.0] * (self.num_events + 1)
        self.next_event_type = 0

        # Counters & Accumulated Metric Areas
        self.numberofcustdelayed_1 = 0
        self.numberofcustdelayed_2 = 0
        self.total_of_delays_1 = 0.0
        self.total_delays_2 = 0.0
        self.area_num_in_q_1 = 0.0
        self.area_num_in_q_2 = 0.0
        self.area_server_status_1 = 0.0
        self.area_server_status_2 = 0.0

        # Run Parameter Variables
        self.avg_interarr = 1.0
        self.avg_svc_1 = 0.9
        self.avg_svc_2 = 0.9
        self.time_ending = 1000.0
        self.outfile_name = "problem14.out"

    def expon(self, mean):
        u = random.random()
        if u == 0.0:
            u = 0.00001
        return -mean * math.log(u)

    def read_parameters(self, input_filename="problem14.in", output_filename="problem14.out"):
        try:
            with open(input_filename, "r") as infile:
                data = infile.read().split()
                if len(data) >= 4:
                    self.avg_interarr = float(data[0])
                    self.avg_svc_1 = float(data[1])
                    self.avg_svc_2 = float(data[2])
                    self.time_ending = float(data[3])
        except FileNotFoundError:
            print(f"Warning: Parameters configuration file {input_filename} missing. Running defaults.")

        self.outfile_name = output_filename

    def initialize(self):
        self.time_clock = 0.0
        self.time_last_event = 0.0
        self.server_status_1 = self.IDLE
        self.server_status_2 = self.IDLE
        self.number_in_q1 = 0
        self.number_in_q2 = 0
        self.arrival_time_1 = []
        self.arrival_time_2 = []
        self.numberofcustdelayed_1 = 0
        self.numberofcustdelayed_2 = 0
        self.total_of_delays_1 = 0.0
        self.total_delays_2 = 0.0
        self.area_num_in_q_1 = 0.0
        self.area_num_in_q_2 = 0.0
        self.area_server_status_1 = 0.0
        self.area_server_status_2 = 0.0

        # Seeds first tracking event configurations
        self.time_next_event[1] = self.time_clock + self.expon(self.avg_interarr)
        self.time_next_event[2] = 1.0e30
        self.time_next_event[3] = 1.0e30
        self.time_next_event[4] = self.time_ending

    def timing(self):
        min_time_next_event = 1.0e29
        self.next_event_type = 0

        for i in range(1, self.num_events + 1):
            if self.time_next_event[i] < min_time_next_event:
                min_time_next_event = self.time_next_event[i]
                self.next_event_type = i

        if self.next_event_type == 0:
            with open(self.outfile_name, "a") as outfile:
                outfile.write(f"\nEvent list empty at time {self.time_clock}")
            sys.exit(1)

        self.time_clock = min_time_next_event

    def update_avgstats_time(self):
        time_since_last_event = self.time_clock - self.time_last_event
        self.time_last_event = self.time_clock

        self.area_num_in_q_1 += self.number_in_q1 * time_since_last_event
        self.area_num_in_q_2 += self.number_in_q2 * time_since_last_event
        self.area_server_status_1 += self.server_status_1 * time_since_last_event
        self.area_server_status_2 += self.server_status_2 * time_since_last_event

    def arrive(self):
        self.time_next_event[1] = self.time_clock + self.expon(self.avg_interarr)

        if self.server_status_1 == self.BUSY:
            self.number_in_q1 += 1
            if self.number_in_q1 > self.Q_LIMIT1:
                with open(self.outfile_name, "a") as outfile:
                    outfile.write(f"\nOverflow array arrival_time_1 time {self.time_clock}")
                sys.exit(2)
            self.arrival_time_1.append(self.time_clock)
        else:
            delay = 0.0
            self.total_of_delays_1 += delay
            self.numberofcustdelayed_1 += 1
            self.server_status_1 = self.BUSY
            self.time_next_event[2] = self.time_clock + self.expon(self.avg_svc_1)

    def depart_1(self):
        if self.number_in_q1 == 0:
            self.server_status_1 = self.IDLE
            self.time_next_event[2] = 1.0e30
        else:
            self.number_in_q1 -= 1
            arrival_time = self.arrival_time_1.pop(0)
            delay = self.time_clock - arrival_time
            self.total_of_delays_1 += delay
            self.numberofcustdelayed_1 += 1
            self.time_next_event[2] = self.time_clock + self.expon(self.avg_svc_1)

        if self.server_status_2 == self.BUSY:
            self.number_in_q2 += 1
            if self.number_in_q2 > self.Q_LIMIT_2:
                with open(self.outfile_name, "a") as outfile:
                    outfile.write(f"\nOverflow of time_arrival_2 array time {self.time_clock}")
                sys.exit(3)
            self.arrival_time_2.append(self.time_clock)
        else:
            delay = 0.0
            self.total_delays_2 += delay
            self.numberofcustdelayed_2 += 1
            self.server_status_2 = self.BUSY
            self.time_next_event[3] = self.time_clock + self.expon(self.avg_svc_2)

    def depart_2(self):
        if self.number_in_q2 == 0:
            self.server_status_2 = self.IDLE
            self.time_next_event[3] = 1.0e30
        else:
            self.number_in_q2 -= 1
            arrival_time = self.arrival_time_2.pop(0)
            delay = self.time_clock - arrival_time
            self.total_delays_2 += delay
            self.numberofcustdelayed_2 += 1
            self.time_next_event[3] = self.time_clock + self.expon(self.avg_svc_2)

    def report(self):
        with open(self.outfile_name, "w") as outfile:
            outfile.write("Problem 1.14\n\n")
            outfile.write(f"Average interarrival time {self.avg_interarr:11.3f} minutes\n\n")
            outfile.write(f"Average service time 1 {self.avg_svc_1:14.3f} minutes\n\n")
            outfile.write(f"Average service time 2 {self.avg_svc_2:14.3f} minutes\n\n")
            outfile.write(f"Duration of simulations {self.time_ending:9.3f} minutes\n\n")
            
            denom1 = self.numberofcustdelayed_1 if self.numberofcustdelayed_1 > 0 else 1
            denom2 = self.numberofcustdelayed_2 if self.numberofcustdelayed_2 > 0 else 1
            
            outfile.write("\n\n")
            outfile.write("Server                                                1        2\n\n")
            outfile.write(f"Delay Average in queue                      {self.total_of_delays_1/denom1:11.3f}{self.total_delays_2/denom2:7.3f} minutes\n\n")
            outfile.write(f"Average number in queue                     {self.area_num_in_q_1/self.time_clock:10.3f}{self.area_num_in_q_2/self.time_clock:7.3f}\n\n")
            outfile.write(f"Server utilization                          {self.area_server_status_1/self.time_clock:10.3f}{self.area_server_status_2/self.time_clock:7.3f}\n\n")
            outfile.write(f"Delays completed                            {self.numberofcustdelayed_1:7d}{self.numberofcustdelayed_2:7d}")

    def run(self):
        self.read_parameters()
        self.initialize()
        while True:
            self.timing()
self.update_avgstats_time()
if self.next_event_type == 1:
self.arrive()
elif self.next_event_type == 2:
self.depart_1()
elif self.next_event_type == 3:
self.depart_2()
elif self.next_event_type == 4:
self.report()
break
if name == "main":
sim = TandemSimulation()
sim.run()
