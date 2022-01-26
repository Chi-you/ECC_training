import pandas as pd
import matplotlib.pyplot as plt
import sys

data = pd.read_csv(sys.argv[1], sep='\s+', header=None)
data = pd.DataFrame(data)

x = data[0]
y = data[1]
plt.yscale("log")
plt.xlabel('Eb/N0')
plt.ylabel('BER')
plt.plot(x, y,'r--')
plt.savefig("BER to Ebn0.png")
plt.show()
