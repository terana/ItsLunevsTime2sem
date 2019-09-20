# You need more of Lunev!

**Problems from MIPT DREC system programming course. 2nd term. **

*Here is the salvation from the nightmare of the bravest of the students of DREC MIPT: solutions to the problems by Lunev.*

## Contents
### 1. List
Implementation of a linked list. Fully covered with tests.

### 2. Integral
Multithreaded integral calculation on a singe host.

The nice feature here is that this program perfectly scales through the number of real cores available. This achieved with analysing the cpu capabilities using procfs and working with OS scheduler.

### 3. Network Integral
Take the programm #2 and make it work on several hosts. Don't forget about scalability!

The hosts communication is organised with TCP and UDP. Full network faults tolerance is achieved.
