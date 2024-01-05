Code from https://www.ida.ing.tu-bs.de/en/artifacts

# This proof-of-concept implementation of SL-LET assumes the following software:
1. ILM (IDA LET Machine): 
    -> Used to generate the LET schedule
    -> Schedules start synchronized by PTP or are synchronized in a macro period
2. Generic Taskset: 
    -> Used to manage tasks for RX and TX part
    -> Activations are either done by ILM (TX) or once at startup (RX)
3. LwIP (Lightweight IP Stack)
    -> Used to transfer UDP packets
4. PTPd
    -> Used to synchronize schedulers and to optionally include timestamp in packets

# An example configuration can be found in cfgTemplate/sllet_config_example.c/h

# Description of an SL-LET Packet
Values in () are optionall and can be enabled by defines
|4 Bytes sequence number     |
|(4 Bytes sequence number)   |
|(8 Bytes tx timestamp)      |
| n Bytes Payload            |

# !Automatic generation of SL-LET configuration and graphical plots not tested!

# Version 1.1, 12.11.19
