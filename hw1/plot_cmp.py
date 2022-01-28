import pandas as pd
import matplotlib.pyplot as plt

data1 = pd.read_csv('Result1.txt', sep='\s+', header=None)
data1 = pd.DataFrame(data1)

data2 = pd.read_csv('Result2.txt', sep='\s+', header=None)
data2 = pd.DataFrame(data2)

x1 = data1[0]
y1 = data1[1]
x2 = data2[0]
y2 = data2[1]
plt.yscale("log")
plt.xlabel('Eb/N0')
plt.ylabel('BER')
plt.plot(x1, y1,'r--', label='uncoded', color=(0, 255/255, 0))
plt.plot(x2, y2,'r--', label='coded')
plt.legend()
plt.savefig("BER to Ebn0.png")
plt.show()
