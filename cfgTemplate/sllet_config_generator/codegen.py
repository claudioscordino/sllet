"""
| Copyright (C) 2019 Phil Hertha
| TU Braunschweig, Germany
| All rights reserved

:Authors:
         - Phil Hertha

Description
-----------

Generate Config for SL LET
"""
tab = "    "


def gen_tx_config(text, t1_Tx):
    text.append(
        "/**\n * TX Configuration for P" + str(1) + ": " + str(t1_Tx) + "ms TX part of interconnect task\n */\n")
    text.append("const SLLET_INTERCONNTECT_INPUT_LABEL sllet_" + str(t1_Tx) + "ms_tx_copy_list[] = {")
    text.append(tab + "/* Task ID, Label ID */")
    text.append(tab + "{.task_id= TASK_" + str(t1_Tx) + "MS_ID, .label_id = 0}")
    text.append("};\n")
    text.append("SLLET_DECLARE_TX_BUFFER(" + str(t1_Tx) + "ms_tx, 12);\n")
    text.append("SLLET_INTERCONNECT_TX_CONFIG sllet_" + str(t1_Tx) + "ms_tx_cfg = {")
    text.append(tab + "SLLET_COPY_LIST(" + str(t1_Tx) + "ms_tx), /* Pointer to copy list */")
    text.append(tab + "SLLET_COPY_LIST_SIZE(" + str(t1_Tx) + "ms_tx), /* Size of Copy List */")
    text.append(tab + "-1, /* socket */")
    text.append(tab + str(1000 + 100) + ", /* UDP Port */")
    text.append(tab + "IPADDR4_INIT_BYTES(192,168,178,10)")
    text.append(tab + "SLLET_TX_BUFFER(" + str(t1_Tx) + "ms_tx)")
    text.append("};\n")


def gen_rx_config(text, t1_Rx, buff, bcrt_com, wcrt_com, delta_R, epsilon):
    text.append("/**\n * RX Configuration for P" + str(1) + ": sporadic RX part of " + str(
        t1_Rx) + "ms interconnect task\n */")
    text.append(
        "SLLET_DECLARE_RX_BUFFER(" + str(t1_Rx) + "ms_rx, SLLET_HEADER_OFFSET + SLLET_COPY_LIST_SIZE(" + str(
            t1_Rx) + "ms_tx));")
    text.append(
        "SLLET_DECLARE_SLLET_BUFFER(" + str(t1_Rx) + "ms_rx," + str(buff) + "*SLLET_COPY_LIST_SIZE(" + str(
            t1_Rx) + "ms_tx));\n")
    text.append("/* Network: T1: " + str(t1_Rx) + "ms, BCRT: " + str(bcrt_com) + "ms, WCRT: " + str(
        wcrt_com) + "ms, Copy-In Phase " + str(delta_R) + "ms,")
    text.append("Sync-Err=" + str(epsilon) + "ms -> N= " + str(buff) + " buffer */")
    text.append("SLLET_BUFFER_CONFIG sllet_" + str(t1_Rx) + "ms_rx_buffer_cfg = {")
    text.append(tab + str(buff) + ", /* Number of entries */")
    text.append(tab + "SLLET_COPY_LIST_SIZE(" + str(t1_Rx) + "ms_tx), /* Number of labels */")
    text.append(tab + "SLLET_SLLET_BUFFER(" + str(t1_Rx) + "ms_rx) /* Buffer base address */")
    text.append("};\n")
    text.append("SLLET_INTERCONNECT_RX_CONFIG sllet_" + str(t1_Rx) + "ms_rx_cfg = {")
    text.append(tab + str(1000 + 100) + ", /* UDP Port */")
    text.append(tab + "SLLET_RX_BUFFER(" + str(t1_Rx) + "ms_rx),")
    text.append(tab + "&sllet_" + str(t1_Rx) + "ms_rx_buffer_cfg, /* Pointer to buffer config */")
    text.append("};\n")


def gen_defines(text):
    text.append("#include \"sllet.h\"\n")
    text.append("#define SLLET_COPY_LIST(TASK) (sllet_##TASK##_copy_list)")
    text.append(
        "#define SLLET_COPY_LIST_SIZE(TASK) (sizeof(sllet_##TASK##_copy_list)/sizeof(SLLET_INTERCONNTECT_INPUT_LABEL))")
    text.append("#define SLLET_DECLARE_TX_BUFFER(TASK, SIZE) uint32_t sllet_##TASK##_txBuffer[SIZE];")
    text.append("#define SLLET_TX_BUFFER(TASK) (sllet_##TASK##_txBuffer)")
    text.append("#define SLLET_DECLARE_RX_BUFFER(TASK, SIZE) uint32_t sllet_##TASK##_rxBuffer[(SIZE)];")
    text.append("#define SLLET_RX_BUFFER(TASK) (sllet_##TASK##_rxBuffer)")
    text.append("#define SLLET_DECLARE_SLLET_BUFFER(TASK, SIZE) uint32_t sllet_##TASK##_slletBuffer[SIZE];")
    text.append("#define SLLET_SLLET_BUFFER(TASK) (sllet_##TASK##_slletBuffer)\n")


def gen_readmap(text, com_Task, T1_Rx, readmap):
    text.append("\n/* Task " + str(com_Task) + "ms read config (Reads from " + str(
        T1_Rx) + "ms SL-LET Task) */\n")
    text.append("const uint16_t task_" + str(com_Task) + "ms_readMap[] = {" + str(readmap)
                + "}; //TODO Generate ReadMap in python\n")
    text.append("SLLET_CONSUMER_CONFIG task_" + str(com_Task) + "ms_cfg = {")
    text.append(tab + "&sllet_" + str(T1_Rx) + "ms_rx_buffer_cfg, /* Pointer to buffer config */")
    text.append(tab + "sizeof(task_" + str(com_Task) + "ms_readMap)/sizeof(uint16_t), /* Read map size */")
    text.append(
        tab + "task_" + str(com_Task) + "ms_readMap /* Read Map for " + str(com_Task) + "ms task    */")
    text.append("};\n")