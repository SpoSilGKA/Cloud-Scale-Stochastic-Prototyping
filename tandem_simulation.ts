📁 3. Finalized TypeScript File (`tandem_simulation.ts`)

```typescript
/**
 * -----------------------------------------------------------------------------
 * GRADUATE COURSEWORK PORTFOLIO: SIMULATION MODELING & ANALYSIS
 * Assignment: Problem 1.14 (Tandem Queue Simulation)
 * Language: TypeScript
 * -----------------------------------------------------------------------------
 * DESCRIPTION:
 * This file provides the complete solution and output metrics for Question 1.14 
 * from my Graduate-level course homework in Simulation Modeling & Analysis. 
 * 
 * METHODOLOGY & THEORETICAL FOUNDATION:
 * Based strictly on the methodologies established in "Simulation Modeling and 
 * Analysis" by Averill M. Law and W. David Kelton. 
 * 
 * CROSS-DOMAIN ARCHITECTURAL ALIGNMENT:
 * Models a two-stage sequential system (Tandem configuration) with separate FIFO 
 * queues. It tracks expected average delay in queue, time-average number of items 
 * in queue, and server utilization over an exact 1000-minute timeline using an 
 * optimized object-oriented event-scheduling mechanism.
 * 
 *   1. SYSTEM PIPELINE (DPU & NIC Context):
 *      Models a low-level network processor pipeline. Stage 1 acts as an Ingress 
 *      Packet Header Parser, which drops packets into an internal buffer queue if 
 *      oversubscribed, before feeding into Stage 2 for Deep Packet Inspection (DPI) 
 *      or hardware-accelerated encryption loops.
 *      
 *   2. BANKING OPERATIONS PARADIGM:
 *      Models a multi-tier branch intake layout. Customers first line up to complete 
 *      initial paperwork and deposits at Stage 1 (Front Teller Lobby), then route 
 *      sequentially to a secondary waiting area for Stage 2 processing (Branch 
 *      Manager/Loan Specialist approvals).
 *      
 *   3. HEALTHCARE MANAGEMENT PATTERN:
 *      Models an urgent care patient flow system. Incoming patients pass through 
 *      Stage 1 (Initial Triage / Vitals Intake Desk), queue up dynamically based 
 *      on arrival rates, and immediately transition to Stage 2 (Physician 
 *      Examination Rooms) for final assessment and discharge.
 * -----------------------------------------------------------------------------
 */

import * as fs from 'fs';

class TandemSimulation {
    private readonly IDLE = 0;
    private readonly BUSY = 1;
    private readonly Q_LIMIT1 = 100;
    private readonly Q_LIMIT_2 = 200;

    private timeClock = 0.0;
    private timeLastEvent = 0.0;
    private serverStatus1 = this.IDLE;
    private serverStatus2 = this.IDLE;
    private numberInQ1 = 0;
    private numberInQ2 = 0;

    private arrivalTime1: number[] = [];
    private arrivalTime2: number[] = [];

    private readonly numEvents = 4;
    private timeNextEvent: number[] = new Array(5).fill(0.0);
    private nextEventType = 0;

    private numberofcustdelayed1 = 0;
    private numberofcustdelayed2 = 0;
    private totalOfDelays1 = 0.0;
    private totalDelays2 = 0.0;
    private areaNumInQ1 = 0.0;
    private areaNumInQ2 = 0.0;
    private areaServerStatus1 = 0.0;
    private areaServerStatus2 = 0.0;

    private avgInterarr = 1.0;
    private avgSvc1 = 0.9;
    private avgSvc2 = 0.9;
    private timeEnding = 1000.0;
    private outfile_name = 'problem14.out';

    private expon(mean: number): number {
        let u = Math.random();
        if (u === 0.0) u = 0.00001;
        return -mean * Math.log(u);
    }

    public readParameters(inputFilename = 'problem14.in', outputFilename = 'problem14.out'): void {
        try {
            const content = fs.readFileSync(inputFilename, 'utf-8');
            const data = content.trim().split(/\s+/);
            if (data.length >= 4) {
                this.avgInterarr = parseFloat(data[0]);
                this.avgSvc1 = parseFloat(data[1]);
                this.avgSvc2 = parseFloat(data[2]);
                this.timeEnding = parseFloat(data[3]);
            }
        } catch (e) {
            console.log(`Warning: Matrix parameters file missing. Using standard defaults.`);
        }
        this.outfile_name = outputFilename;
    }

    private initialize(): void {
        this.timeClock = 0.0;
        this.timeLastEvent = 0.0;
        this.serverStatus1 = this.IDLE;
        this.serverStatus2 = this.IDLE;
        this.numberInQ1 = 0;
        this.numberInQ2 = 0;
        this.arrivalTime1 = [];
        this.arrivalTime2 = [];
        this.numberofcustdelayed1 = 0;
        this.numberofcustdelayed2 = 0;
        this.totalOfDelays1 = 0.0;
        this.totalDelays2 = 0.0;
        this.areaNumInQ1 = 0.0;
        this.areaNumInQ2 = 0.0;
        this.areaServerStatus1 = 0.0;
        this.areaServerStatus2 = 0.0;

        this.timeNextEvent[1] = this.timeClock + this.expon(this.avgInterarr);
        this.timeNextEvent[2] = 1.0e30;
        this.timeNextEvent[3] = 1.0e30;
        this.timeNextEvent[4] = this.timeEnding;
    }

    private timing(): void {
        let minTimeNextEvent = 1.0e29;
        this.nextEventType = 0;

        for (let i = 1; i <= this.numEvents; i++) {
            if (this.timeNextEvent[i] < minTimeNextEvent) {
                minTimeNextEvent = this.timeNextEvent[i];
                this.nextEventType = i;
            }
        }

        if (this.nextEventType === 0) {
            fs.appendFileSync(this.outfile_name, `\nEvent list empty at time ${this.timeClock}`);
            process.exit(1);
        }

        this.timeClock = minTimeNextEvent;
    }

    private updateAvgStatsTime(): void {
        const timeSinceLastEvent = this.timeClock - this.timeLastEvent;
        this.timeLastEvent = this.timeClock;

        this.areaNumInQ1 += this.numberInQ1 * timeSinceLastEvent;
        this.areaNumInQ2 += this.numberInQ2 * timeSinceLastEvent;
        this.areaServerStatus1 += this.serverStatus1 * timeSinceLastEvent;
        this.areaServerStatus2 += this.serverStatus2 * timeSinceLastEvent;
    }

    private arrive(): void {
        this.timeNextEvent[1] = this.timeClock + this.expon(this.avgInterarr);

        if (this.serverStatus1 === this.BUSY) {
            this.numberInQ1++;
            if (this.numberInQ1 > this.Q_LIMIT1) {
                fs.appendFileSync(this.outfile_name, `\nOverflow array arrival_time_1 time ${this.timeClock}`);
                process.exit(2);
            }
            this.arrivalTime1.push(this.timeClock);
        } else {
            const delay = 0.0;
            this.totalOfDelays1 += delay;
            this.numberofcustdelayed1++;
            this.serverStatus1 = this.BUSY;
            this.timeNextEvent[2] = this.timeClock + this.expon(this.avgSvc1);
        }
    }

    private depart1(): void {
        if (this.numberInQ1 === 0) {
            this.serverStatus1 = this.IDLE;
            this.timeNextEvent[2] = 1.0e30;
        } else {
            this.numberInQ1--;
            const arrivalTime = this.arrivalTime1.shift()!;
            const delay = this.timeClock - arrivalTime;
            this.totalOfDelays1 += delay;
            this.numberofcustdelayed1++;
            this.timeNextEvent[2] = this.timeClock + this.expon(this.avgSvc1);
        }

        if (this.serverStatus2 === this.BUSY) {
            this.numberInQ2++;
            if (this.numberInQ2 > this.Q_LIMIT_2) {
                fs.appendFileSync(this.outfile_name, `\nOverflow of time_arrival_2 array time ${this.timeClock}`);
                process.exit(3);
            }
            this.arrivalTime2.push(this.timeClock);
        } else {
            const delay = 0.0;
            this.totalDelays2 += delay;
            this.numberofcustdelayed2++;
            this.serverStatus2 = this.BUSY;
            this.timeNextEvent[3] = this.timeClock + this.expon(this.avgSvc2);
        }
    }

    private depart2(): void {
        if (this.numberInQ2 === 0) {
            this.serverStatus2 = this.IDLE;
            this.timeNextEvent[3] = 1.0e30;
        } else {
            this.numberInQ2--;
            const arrivalTime = this.arrivalTime2.shift()!;
            const delay = this.timeClock - arrivalTime;
            this.totalDelays2 += delay;
            this.numberofcustdelayed2++;
            this.timeNextEvent[3] = this.timeClock + this.expon(this.avgSvc2);
        }
    }

    private report(): void {
        const denom1 = this.numberofcustdelayed1 > 0 ? this.numberofcustdelayed1 : 1;
        const denom2 = this.numberofcustdelayed2 > 0 ? this.numberofcustdelayed2 : 1;

        let reportText = `Problem 1.14\n\n`;
        reportText += `Average interarrival time ${this.avgInterarr.toFixed(3)} minutes\n\n`;
        reportText += `Average service time 1 ${this.avgSvc1.toFixed(3)} minutes\n\n`;
        reportText += `Average service time 2 ${this.avgSvc2.toFixed(3)} minutes\n\n`;
        reportText += `Duration of simulations ${this.timeEnding.toFixed(3)} minutes\n\n\n\n`;
        reportText += `Server                                                1        2\n\n`;
        reportText += `Delay Average in queue                      ${(this.totalOfDelays1 / denom1).toFixed(3).padStart(11)}${(this.totalDelays2 / denom2).toFixed(3).padStart(7)} minutes\n\n`;
        reportText += `Average number in queue                     ${(this.areaNumInQ1 / this.timeClock).toFixed(3).padStart(10)}${(this.areaNumInQ2 / this.timeClock).toFixed(3).padStart(7)}\n\n`;
        reportText += `Server utilization                          ${(this.areaServerStatus1 / this.timeClock).toFixed(3).padStart(10)}${(this.areaServerStatus2 / this.timeClock).toFixed(3).padStart(7)}\n\n`;
        reportText += `Delays completed                            ${this.numberofcustdelayed1.toString().padStart(7)}${this.numberofcustdelayed2.toString().padStart(7)}`;

        fs.writeFileSync(this.outfile_name, reportText);
    }

    public run(): void {
        this.readParameters();
        this.initialize();
        while (true) {
            this.timing();
            this.updateAvgStatsTime();
            if (this.nextEventType === 1) {
this.arrive();
} else if (this.nextEventType === 2) {
this.depart1();
} else if (this.nextEventType === 3) {
this.depart2();
} else if (this.nextEventType === 4) {
this.report();
break;
}
}
}
}
const sim = new TandemSimulation();
sim.run();
