import matplotlib.patches as patches
from matplotlib.offsetbox import AnchoredText
from pylab import *

""" Stucture of csv file
# 0    1    2      3       4       5                   6
# let1, BCRT, let_Phi, EPSILON, DELTA_R, Com_READER_PERIOD, let2
5000,0,6000,200,200,10000,5000
2000,0,5000,200,200,1000,2000
"""

# This Script will frist generate the ylabels
def plot_gant(let_period, bcrt, let_Phi, epsilon, delta_r, intercom_period, buff, len_readmap, readmap, n_exec):

    ylabels = []
    timing = []
    task_timing = {}

    # Definition of colors for bars
    tu_red = '#be1e3c'  # TU RED
    tu_green = '#6d8300'
    tu_greenlight = '#a7b566'
    tu_blue = '#00709b'
    tu_bluelight = '#66a9c3'
    tu_orange = '#e16d00'
    tu_orangelight = '#eda766'
    tu_violett = '#511246'
    tu_violettlight = '#977190'
    # This 2d Color Array consists of an "base color" and i light version, so we can print bcrt and let_Phi better
    colors = [[tu_blue, tu_bluelight], [tu_green, tu_greenlight], [tu_orange, tu_orangelight], [tu_violett, tu_violettlight]]
    # Alpha value for arrows. bars are always 100% visible
    alpha = 0.8

    # Enable TeX in Maplotlib
    plt.rc('text', usetex=True)
    plt.rc('font', family='serif')

    fig, ax = plt.subplots(figsize=(20, 8))

    # Generate comTask plot
    # Generate as many plots as the readmap is long
    # TODO: Doesnt work for undersampling, just for oversampling
    range_let = 0
    if intercom_period >= let_period:
        range_let = len_readmap
    else:
        range_let = len_readmap * int(math.ceil(intercom_period * let_period))

    for j in range(0, range_let):
        if j == 0:
            ylabel = r'$\phi_{i,q} $'
        else:
            ylabel = r'$\phi_{i,q+' + str(j) + '} $'
        start = intercom_period * j
        end = intercom_period * j + let_Phi
        ylabels.append(ylabel)
        timing.append([start, end])

    # Generate label plot
    for j in range(0, range_let):
        if j == 0:
            ylabel = r'$\l_{i,q} $'
        else:
            ylabel = r'$\l_{i,q+' + str(j) + '} $'
        start = intercom_period * j + bcrt
        end = (j + 1) * intercom_period + let_Phi + delta_r
        ylabels.append(ylabel)
        timing.append([start, end])

    # Add LET Task 2/Rx
    ylabel = r'$\lambda_2 $'
    start = 0
    end = let_period
    ylabels.append(ylabel)
    timing.append([start, end])

    max_x = intercom_period * (len_readmap) + let_Phi
    max_y = len(ylabels) - 1
    j_com = let_Phi - bcrt
    firstrun = True
    first = True
    readmap = []
    color_index_label = 0
    color_index_com = 0
    buff_index = 0

    for j, task in enumerate(ylabels):
        task_timing[task] = timing[j]

    # iterate through every let config and create separate plots
    for h in range(len(ylabels)):
        start, end = task_timing[ylabels[h]]
        # Define an uniform Size for bcrt/let_Phi/... marker. This size is dependend on the size of the graph
        marker = max_x * 0.003
        # Scale for the annotated text of label plot
        text_scale = end - start

        # Start Plotting com task part
        if "phi" in ylabels[h]:
            cur_bcrt = start + bcrt
            cur_let_Phi = start + let_Phi
            # Print bar till bcrt
            ax.barh(h, bcrt, left=start, height=0.25, align='center', color=colors[color_index_com][0], alpha=1, antialiased=True)
            # print bar starting from bcrt to let_Phi in diff color
            ax.barh(h, j_com, left=cur_bcrt, height=0.25, align='center', color=colors[color_index_com][1], alpha=1, antialiased=True)
            if buff > len(colors):
                color_index_com = (color_index_com + 1) % len(colors)
            else:
                color_index_com = (color_index_com + 1) % buff

            # Add marker for BCRT
            ax.barh(h, marker, left=cur_bcrt - marker / 2, height=0.25, align='center', color='black', alpha=1, antialiased=True)
            # Add marker for let_Phi
            ax.barh(h, marker, left=cur_let_Phi - marker / 2, height=0.25, align='center', color='black', alpha=1, antialiased=True)

            # Annotate text for bcrt and let_Phi
            if ",0" in ylabels[h]:
                plt.annotate("BCRT", xy=(cur_bcrt, h + 0.1), xytext=(cur_bcrt + 0.5, h + 0.6), fontsize=12,
                             arrowprops=dict(facecolor='black', alpha=alpha, arrowstyle='->'))
                #plt.annotate("let\_Phi", xy=(cur_let_Phi, h - 0.1), xytext=(cur_let_Phi - 0.5, h - 0.4), fontsize=12,
                #             arrowprops=dict(facecolor='black', alpha=alpha, arrowstyle='->'))

            # print publishing of label, which buffer will be writen to?
            plt.annotate("", xy=(cur_let_Phi, h + len_readmap - 0.1), xytext=(cur_let_Phi, h + 0.1),
                         arrowprops=dict(color='black', alpha=alpha, arrowstyle='-', linewidth=2, linestyle=":"))

        # Start Plotting labels
        if 'l_{' in ylabels[h]:
            t3 = start + j_com + delta_r
            if ',q}' in ylabels[h] and 'l_' in ylabels[h]:
                # Annotate the first label graph and add text for t1 - t4
                plt.annotate(r'$t_1(q) $', xy=(start, h + 0.05), xytext=(start + text_scale * 0.05, h + 0.5), fontsize=12,
                             arrowprops=dict(facecolor='black', alpha=alpha, arrowstyle='->'))
                plt.annotate(r'$t_2(q) $', xy=(start + j_com, h - 0.05), xytext=(start + j_com - 0.09375 * text_scale, h - 0.5), fontsize=12,
                             arrowprops=dict(facecolor='black', alpha=alpha, arrowstyle='->'))
                plt.annotate(r'$t_3(q) $', xy=(t3 + text_scale * 0.015625, h - 0.05), xytext=(t3 + text_scale * 0.033333333, h - 0.5), fontsize=12,
                             arrowprops=dict(facecolor='black', alpha=alpha, arrowstyle='->'))
                plt.annotate(r'$t_4(q) $', xy=(end + 0.1, h), fontsize=12, xytext=(end + text_scale * 0.033333333, h + 0.5),
                             arrowprops=dict(facecolor='black', alpha=alpha, arrowstyle='->'))

            # plot the whole bar starting at bcrt of current comtask to let_Phi of second task
            ax.barh(h, end - start, left=start, height=0.25 * 0.5, align='center', color=colors[color_index_label][0], alpha=1, antialiased=True)
            # plot marker for t1
            ax.barh(h, marker, left=start - marker / 2, height=0.25, align='center', color='black', alpha=1, antialiased=True)
            # plot marker for t2
            ax.barh(h, marker, left=start + j_com - marker / 2, height=0.25, align='center', color='black', alpha=1, antialiased=True)
            # plot marker for t4
            ax.barh(h, marker, left=end - marker / 2, height=0.25, align='center', color='black', alpha=1, antialiased=True)
            # plot marker for t3 as triangle
            X = np.array([[t3 + 0.015625 * text_scale, h], [t3, h - 0.12], [t3, h + 0.12]])
            poly = plt.Polygon(X, facecolor='None', edgecolor='black', linewidth=2)
            plt.gca().add_patch(poly)
            n = int(t3 / let_period)
            # new labels are published with every exec of com task,
            # check if next publishing is before the next activation of let task 2
            sum_list = buff
            if t3 + intercom_period > (n + 1) * let_period:
                armB = max_y - h
                armA = t3 - n * let_period
                arrow = patches.FancyArrowPatch((t3 + 0.015625 * text_scale / 2, h + 0.1), ((n + 1) * let_period, max_y - 0.2),
                                                connectionstyle="bar, angle=180, armA=" + str(armA) + ", armB=" + str(armB),
                                                arrowstyle='->', linewidth=2, color='black', alpha=alpha, linestyle=':')
                plt.gca().add_patch(arrow)
                # Color LET Task 2 in corresponding color. Scale is added to the bar, so the black activation bars wont get repainted
                while True:
                    for m in range(1, len(ylabels)):
                        if t3 + intercom_period > (n + m) * let_period:
                            # check if next publish time is before next let activation
                            ax.barh(max_y, let_period - marker, left=(n + m) * let_period + marker / 2, height=0.25, align='center',
                                    color=colors[color_index_label][0], alpha=1, antialiased=True)
                            # Calculation of Readmap
                            if first == True:
                                skipped_let = n + m
                                first = False
                            try:
                                readmap.index(buff_index)
                            except ValueError:
                                sum_list -= 1
                            if len(readmap) == 0:
                                readmap.append(buff_index)
                            elif (readmap[len(readmap) - 1] == buff_index or sum_list > 0) and len(readmap) < len_readmap:
                                readmap.append(buff_index)
                        else:
                            break
                    break
            buff_index = (buff_index + 1) % buff
            # TODO: Add more colors, so every buffer has its own color
            if buff > len(colors):
                color_index_label = (color_index_label + 1) % len(colors)
            else:
                color_index_label = (color_index_label + 1) % buff

        else:
            """ Start Plotting LET Tasks"""
            if firstrun:
                # Add LET Task 2
                x = len(ylabels) - 1
                ax.barh(x, max_x, left=0, height=0.25, align='center', color=tu_red, alpha=1, antialiased=True)
                for g in range(0, int(max_x / let_period) + 1):
                    ax.barh(x, marker, left=g * let_period - marker / 2, height=0.25, align='center', color='black', alpha=1, antialiased=True)
                    # Arrow size depends on the size of the plot
                    plt.annotate("", xy=(g * let_period, x - 0.11), xytext=(g * let_period, x - 0.057 * x), arrowprops=dict(facecolor='black', shrink=0.01))
                firstrun = False

    # Sort readmap and start with job 0
    readmap_sort = []
    for x in range(len(readmap) - skipped_let, len(readmap)):
        readmap_sort.append(readmap[x])

    for x in range(0, len(readmap) - skipped_let):
        readmap_sort.append(readmap[x])

    readmap = readmap_sort

    # Setting for grid and axis
    ax.grid(color='g', linestyle=':', linewidth=0.5)
    ax.invert_yaxis()
    ax.set_yticks(range(len(ylabels)))
    if len(ylabels) > 15:
        fontsize=14
    elif len(ylabels) > 20:
        fontsize=10
    else:
        fontsize = 16
    ax.set_yticklabels(ylabels, fontsize=fontsize)
    ax.set_xlabel("time [ms]")
    ax.set_title("System Level LET")
    plt.tight_layout()

    # Add an Textbox in upper right
    box_text = "Readmap: " + str(readmap)
    box_text += "\nConsumer Task: " + str(let_period) + " ms"
    box_text += "\nComTask: " + str(intercom_period) + " ms"
    box_text += "\nBCRT: " + str(bcrt) + " ms"
    box_text += "\n$LET_\Phi$: " + str(let_Phi) + " ms"
    box_text += "\nLen rm: " + str(len_readmap)
    box_text += "\nBuffersize: " + str(buff)
    text_box = AnchoredText(box_text, frameon=True, loc=7, pad=0.5)
    plt.setp(text_box.patch, facecolor='white', alpha=1)
    ax.add_artist(text_box)

    # plot graph as pdf
    filename = 'sllet' + str(n_exec) + '.pdf'
    plt.savefig(filename)

    # Clear variables to create an second figure
    ylabels.clear()
    timing.clear()
    task_timing.clear()
    skipped_let = 0

    return readmap


if __name__ == "__main__":
    plot_gant()
