import pandas as pd
import matplotlib.pyplot as plt

plt.style.use("seaborn-deep")

# date = "2020-06-18 14:46:35"
# date = "2020-06-18 15:01:02"  # high frequency test
# date = "2020-06-18 15:10:06"  # low frequency
# date = "2020-06-18 18:01:41"    # filtered
# date = "2020-06-18 18:07:51"      # filtered cycle test
# date = "2020-06-18 18:14:02"    # filtered, frequency test
# date = "2020-06-18 18:19:12"     # max force test
# date = "2020-06-18 19:08:09"    # para 9, 333hz, freq test
# date = "2020-06-19 13:37:47"
# date = "2020-06-19 13:59:48"
# date = "2020-06-19 14:07:45"
date = "2020-06-19 14:12:12"
fmri = pd.read_excel("./../other/data/" + date + "_nn.xlsx")
fmri2 = pd.read_excel("./../other/data/" + date + "_spi_command.xlsx")
# fmri = sns.load_dataset("./../other/data/" + date + "_nn.xlsx")
# ax = sns.lineplot(x="time", y="input40", data=fmri)
# ax.lineplot(x="time",y="q2")
fig, ax = plt.subplots(2, 2)
ax[0, 0].plot(fmri.time[2:], fmri.output0[2:], color='C0')
ax[0, 0].plot(fmri.time[2:], fmri.output1[2:], color='C1')
ax[0, 0].plot(fmri.time[2:], fmri.output2[2:], color='C2')
ax[0, 0].plot(fmri2.time, fmri2.q0, color='C0', linestyle='--')
ax[0, 0].plot(fmri2.time, fmri2.q1, color='C1', linestyle='--')
ax[0, 0].plot(fmri2.time, fmri2.q2, color='C2', linestyle='--')

ax[0, 1].plot(fmri.time[2:], fmri.output3[2:], color='C0')
ax[0, 1].plot(fmri.time[2:], fmri.output4[2:], color='C1')
ax[0, 1].plot(fmri.time[2:], fmri.output5[2:], color='C2')
ax[0, 1].plot(fmri2.time, fmri2.q3, color='C0', linestyle='--')
ax[0, 1].plot(fmri2.time, fmri2.q4, color='C1', linestyle='--')
ax[0, 1].plot(fmri2.time, fmri2.q5, color='C2', linestyle='--')

ax[1, 0].plot(fmri.time[2:], fmri.output6[2:], color='C0')
ax[1, 0].plot(fmri.time[2:], fmri.output7[2:], color='C1')
ax[1, 0].plot(fmri.time[2:], fmri.output8[2:], color='C2')
ax[1, 0].plot(fmri2.time, fmri2.q6, color='C0', linestyle='--')
ax[1, 0].plot(fmri2.time, fmri2.q7, color='C1', linestyle='--')
ax[1, 0].plot(fmri2.time, fmri2.q8, color='C2', linestyle='--')

ax[1, 1].plot(fmri.time[2:], fmri.output9[2:], color='C0')
ax[1, 1].plot(fmri.time[2:], fmri.output10[2:], color='C1')
ax[1, 1].plot(fmri.time[2:], fmri.output11[2:], color='C2')
ax[1, 1].plot(fmri2.time, fmri2.q9, color='C0', linestyle='--')
ax[1, 1].plot(fmri2.time, fmri2.q10, color='C1', linestyle='--')
ax[1, 1].plot(fmri2.time, fmri2.q11, color='C2', linestyle='--')

plt.show()
