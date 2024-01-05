"""
 * Copyright 2019 TU Braunschweig, Institute of Computer and Network Engineering
 * Contributors: Kai-Björn Gemlau (gemlau@ida.ing.tu-bs.de)
 *               Phil Hertha
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the 
 * documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its 
 * contributors may be used to endorse or promote products derived 
 * from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.

Description
-----------
Generate Config for SL LET
This Programm Parses an csv-file for sllet Parameters and generates
an sllet-config file.
The csv-File has the following structure:
Rx_Period,BCRT,WCRT,EPSILON,DELTA_R,COM_READER_PERIOD,TX_PERIOD

"""
import plot_sllet
import codegen
from pylab import *

time_scale = 0.001
global readmap
readmap = []


def gen_text(sllet_task, bcrt, wcrt, epsilon, delta_R, com_Task, buff, readmap):
    text = []
    """ Generate Defines """
    codegen.gen_defines(text)

    """ Generate Tx Configuration """
    codegen.gen_tx_config(text, sllet_task)

    """ Generate Rx Configuration"""
    codegen.gen_rx_config(text, sllet_task, buff, bcrt, wcrt, delta_R, epsilon)

    """ Generate Read Map/ interconnect Task"""
    print(com_Task)
    codegen.gen_readmap(text, com_Task, sllet_task, readmap)

    return text


def parse_rx_config():
    file = "tasks.csv"
    file_data = np.genfromtxt(file, delimiter=',', dtype=None, names=True, comments='#',
                              usecols=(0, 1, 2, 3, 4, 5))
    data = list()
    for line in file_data:
        data.append(line)
    return data


def main():
    """
    Calculate Size of Buffer
    N = 1 + ceil((LET_Phi + Delta_R - BCRT + epsilon) / T_Phi)
    Calculate Size of Readmap
    r = T1 * N / T3
    """

    #buff = []
    T_2 = []
    bcrt = []
    let_Phi = []
    epsilon = []
    delta_R = []
    T_Phi = []
    #len_readmap = []

    # 0    1    2      3       4       5                   6
    # BCRT, LET, EPSILON, DELTA_R, T, T_Consumer
    """Parse csv-file"""
    config = parse_rx_config()
    for data in config:
        bcrt.append(float(data[0])*time_scale)
        let_Phi.append(float(data[1])*time_scale)
        epsilon.append(float(data[2])*time_scale)
        delta_R.append(float(data[3])*time_scale)
        T_Phi.append(float(data[4])*time_scale)
        T_2.append(float(data[5])*time_scale)

    for i in range(0, len(T_2)):
        readmap = []

        """Calc Buffer Size for every task"""
        buff = int(math.ceil(1 + (let_Phi[i] - bcrt[i] + delta_R[i] + epsilon[i]) / T_Phi[i]))
        length_readmap = math.ceil(buff * T_Phi[i] / T_2[i])

        readmap = plot_sllet.plot_gant(T_2[i], bcrt[i], let_Phi[i], epsilon[i], delta_R[i], T_Phi[i], buff, length_readmap, readmap, i)

        readmap_str = ""
        first = True
        for j in range(0, len(readmap)):
            if first == True:
                readmap_str += str(readmap[j])
                first = False
            else:
                readmap_str += " ," + str(readmap[j])

        text = gen_text(int(T_Phi[i]), int(bcrt[i]), int(let_Phi[i]), int(epsilon[i]), int(delta_R[i]), int(T_2[i]), int(buff), readmap_str)


        """ Write text to File """
        with open("test" + str(i) + ".c", "w") as text_file:
            for j in range(len(text)):
                text_file.write(text[j] + "\n")
        text_file.close()
        readmap.clear()


if __name__ == "__main__":
    main()
