import math
import matplotlib.pyplot as plt
import numpy as np

def f(x):
	v = 1.1 - np.abs ( x - 0.45 ) / 0.1
	return [min(1.0,max(0.0,y)) for y in v]

t1 = np.arange ( 0.0, 1.0, 0.01 )
#t2 = np.arange ( 0.0, 5.0, 0.02 )

#plt.plot([1, 2, 3, 4])
fig=plt.plot(t1, f(t1))
plt.axes ( [0,1,0,1])
#fig.axes.get_xaxis().set_visible(False)
#fig.axes.get_yaxis().set_visible(False)
#plt.plot([1, 2, 3, 4], [1, 4, 9, 16], 'ro')
#plt.ylabel('some numbers')
#plt.axis ( [0,6,0,20])
plt.title ( 'EmberGlow' )
plt.xticks([])
plt.show()